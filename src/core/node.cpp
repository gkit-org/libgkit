#include "core/node_internal.hpp"
#include "core/object_pool.hpp"
#include "gkit/core/reflect/registry.hpp"

#include <algorithm>

namespace gkit::core::detail {
    std::mutex tree_mutex;

    auto NodeState::resolve(ObjectId id) -> NodeState* {
        return ObjectPool::instance().find_node(id);
    }


    auto NodeState::find(ObjectId child) -> std::vector<UniqueObject>::iterator {
        return std::ranges::find(children, child, &UniqueObject::get_id);
    }


    auto NodeState::append(const std::string& key, UniqueObject& child) -> bool {
        auto [it, inserted] = child_names.emplace(key, child.get_id());
        if (!inserted) return false;
        try {
            children.push_back(std::move(child));
        } catch (...) {
            child_names.erase(it);
            throw;
        }
        return true;
    }


    // Release registrations outside the topology lock, even if application callbacks still pin a node.
    auto NodeState::retire() noexcept -> void {
        std::vector<UniqueObject> released;
        {
            std::lock_guard guard(tree_mutex);
            if (retired) return;
            retired = true;
            child_names.clear();
            released.swap(children);
            for (std::size_t i = 0; i < released.size(); ++i) {
                auto& child   = of(*static_cast<Node*>(released[i].get()));
                child.retired = true;
                child.parent  = {};
                child.runtime = nullptr;
                for (auto& owner : child.children) released.push_back(std::move(owner));
                child.child_names.clear();
                child.children.clear();
            }
        }
    }


    // Stable topology during coordinator traversal. Parent links keep deep-tree cleanup iterative.
    template<class Before, class After>
    static auto walk(NodeState* root, Before before, After after) -> void {
        auto* current = root;
        before(*current);
        current->traversal_index = 0;
        for (;;) {
            if (current->traversal_index < current->children.size()) {
                current                  = current->descendants()[current->traversal_index++];
                current->traversal_index = 0;
                before(*current);
            } else {
                after(*current);
                if (current == root) return;
                current = NodeState::resolve(current->parent);
            }
        }
    }


    auto assign_runtime(NodeState* root, TreeRuntime* runtime) -> void {
        if (root->runtime == runtime) return;
        walk(
            root,
            [runtime](NodeState& state) -> void {
                state.runtime = runtime;
            },
            [](NodeState&) -> void {});
    }


    auto enter_nodes(NodeState* root) -> void {
        walk(
            root,
            [](NodeState& state) -> void {
                state.entered = true;
                state.node.enter_tree();
            },
            [](NodeState& state) -> void {
                if (!state.prepared) {
                    state.prepared = true;
                    state.node.ready();
                }
            });
    }


    auto exit_nodes(NodeState* root, std::exception_ptr& failure) noexcept -> void {
        walk(
            root,
            [](NodeState&) -> void {},
            [&](NodeState& state) -> void {
                if (!state.entered) return;
                state.entered = false;
                try {
                    state.node.exit_tree();
                } catch (...) {
                    if (!failure) failure = std::current_exception();
                }
            });
    }


    auto Edit::submit() -> void {
        std::unique_lock guard(tree_mutex);
        submit(guard);
    }


    auto Edit::submit(std::unique_lock<std::mutex>& guard) -> void {
        auto* origin  = NodeState::resolve(anchor);
        auto* runtime = origin ? origin->runtime : nullptr;
        if (runtime && runtime->active) {
            {
                std::lock_guard queue_guard(runtime->queue_mutex);
                if (runtime->accepting) {
                    runtime->edits.push_back(std::move(*this));
                    return;
                }
            }
            apply(runtime, guard, true);
        } else {
            apply(runtime, guard, false);
        }
    }


    auto Edit::apply(TreeRuntime* runtime, std::unique_lock<std::mutex>& guard, bool cancel) -> void {
        auto* origin       = NodeState::resolve(anchor);
        auto* other        = NodeState::resolve(target);
        NodeError error    = NodeError::None;
        NodeState* added   = nullptr;
        NodeState* removed = nullptr;
        if (cancel)
            error = NodeError::Stopped;
        else if (!origin || origin->retired || origin->runtime != runtime)
            error = NodeError::Expired;
        else {
            switch (kind) {
            case EditKind::Add:
            case EditKind::SetChild: {
                if (!other || other->retired || other->parent.available() || other->runtime || name.empty()) {
                    error = NodeError::InvalidEdit;
                    break;
                }
                // A detached owner can be passed to its own descendant.
                for (auto* p = origin; p; p = NodeState::resolve(p->parent))
                    if (p == other) {
                        error = NodeError::InvalidEdit;
                        break;
                    }
                if (error != NodeError::None) break;
                if (kind == EditKind::SetChild && origin->managed_child.available()) {
                    auto slot                    = origin->find(origin->managed_child);
                    removed                      = NodeState::resolve(origin->managed_child);
                    name                         = removed->name;
                    origin->child_names.at(name) = target;
                    std::swap(*slot, *owner);
                    removed->parent = {};
                } else if (!origin->append(name, *owner)) {
                    error = NodeError::InvalidEdit;
                    break;
                }
                if (!removed) owner.reset();
                if (kind == EditKind::SetChild) origin->managed_child = target;
                other->parent = anchor;
                other->name   = std::move(name);
                assign_runtime(other, runtime);
                added = other;
                break;
            }
            case EditKind::Rename: {
                auto* parent = NodeState::resolve(origin->parent);
                if (name.empty()) {
                    error = NodeError::InvalidEdit;
                    break;
                }
                if (origin->name == name) break;
                if (parent) {
                    if (!parent->child_names.emplace(name, anchor).second) {
                        error = NodeError::InvalidEdit;
                        break;
                    }
                    parent->child_names.erase(origin->name);
                }
                origin->name = std::move(name);
                break;
            }
            case EditKind::Reparent: {
                auto* parent = NodeState::resolve(origin->parent);
                if (!parent || !other || other->retired || other->runtime != runtime) {
                    error = NodeError::InvalidEdit;
                    break;
                }
                if (parent == other) break;
                auto* top = other;
                while (top != origin && top->parent.available()) top = NodeState::resolve(top->parent);
                auto* root = parent;
                while (root->parent.available()) root = NodeState::resolve(root->parent);
                if (top != root) {
                    error = NodeError::InvalidEdit;
                    break;
                }
                auto it = parent->find(anchor);
                if (!other->append(origin->name, *it)) {
                    error = NodeError::InvalidEdit;
                    break;
                }
                parent->child_names.erase(origin->name);
                parent->children.erase(it);
                if (parent->managed_child == anchor) parent->managed_child = {};
                origin->parent = target;
                break;
            }
            case EditKind::Detach:
            case EditKind::Remove: {
                auto it = origin->find(target);
                if (it == origin->children.end()) {
                    error = NodeError::InvalidEdit;
                    break;
                }
                owner.emplace(std::move(*it));
                origin->child_names.erase(other->name);
                origin->children.erase(it);
                if (origin->managed_child == target) origin->managed_child = {};
                other->parent = {};
                removed       = other;
                break;
            }
            }
        }
        bool running = runtime && runtime->active;
        // A stopped tree can be edited by another thread as soon as we unlock.
        if (removed && !running) assign_runtime(removed, nullptr);
        guard.unlock();
        std::exception_ptr failure;
        if (removed && running) {
            exit_nodes(removed, failure);
            std::lock_guard lock(tree_mutex);
            assign_runtime(removed, nullptr);
        }
        if (added && running && !failure) {
            try {
                enter_nodes(added);
            } catch (...) {
                failure = std::current_exception();
            }
        }
        if (kind == EditKind::Remove || (kind == EditKind::SetChild && removed)) owner.reset();
        if (complete) {
            try {
                complete({error, std::move(owner)});
            } catch (...) {
                if (!failure) failure = std::current_exception();
            }
        }
        if (failure) std::rethrow_exception(failure);
    }


    auto drain_edits(TreeRuntime& runtime, bool cancel) -> void {
        std::deque<Edit> edits;
        {
            std::lock_guard guard(runtime.queue_mutex);
            edits.swap(runtime.edits);
        }
        std::exception_ptr failure;
        for (auto& edit : edits) {
            try {
                std::unique_lock guard(tree_mutex);
                edit.apply(&runtime, guard, cancel || bool(failure));
            } catch (...) {
                if (!failure) failure = std::current_exception();
            }
        }
        if (failure) std::rethrow_exception(failure);
    }


    auto drain_posts(TreeRuntime& runtime, bool cancel) -> void {
        std::deque<TreeRuntime::Post> posts;
        {
            std::lock_guard guard(runtime.queue_mutex);
            posts.swap(runtime.posts);
        }
        std::exception_ptr failure;
        for (auto& post : posts) {
            auto error = NodeError::Stopped;
            try {
                if (!cancel && !failure) {
                    error = NodeError::Expired;
                    post.target.with<Node>([&](Node& node) -> void {
                        {
                            std::lock_guard guard(tree_mutex);
                            if (NodeState::of(node).runtime != &runtime) return;
                        }
                        post.callback(node);
                        error = NodeError::None;
                    });
                }
            } catch (...) {
                error   = NodeError::CallbackFailed;
                failure = std::current_exception();
            }
            try {
                if (post.complete) post.complete(error);
            } catch (...) {
                if (!failure) failure = std::current_exception();
            }
        }
        if (failure) std::rethrow_exception(failure);
    }
} // namespace gkit::core::detail

namespace gkit::core {
    auto Node::_regist_method(reflect::ClassDB& db) -> void {
        db.regist<Node>("Node").add_property("Node", "name", &Node::get_name, &Node::set_name);
    }


    Node::Node() : state(std::make_unique<detail::NodeState>(*this)) {}


    Node::~Node() = default;


    auto Node::id() const noexcept -> ObjectId {
        return state->id;
    }


    auto Node::parent() const -> std::optional<ObjectId> {
        std::lock_guard guard(detail::tree_mutex);
        if (state->parent.available()) return state->parent;
        return {};
    }


    auto Node::find_ancestor_if(bool (*matches)(const Node&)) const -> std::optional<ObjectId> {
        std::lock_guard guard(detail::tree_mutex);
        for (auto id = state->parent; id.available();) {
            auto* ancestor = detail::NodeState::resolve(id);
            if (!ancestor) break;
            if (matches(ancestor->node)) return id;
            id = ancestor->parent;
        }
        return std::nullopt;
    }


    auto Node::name() const -> std::string {
        std::lock_guard guard(detail::tree_mutex);
        return state->name;
    }


    auto Node::find_child(const std::string& name) const -> std::optional<ObjectId> {
        std::lock_guard guard(detail::tree_mutex);
        auto it = state->child_names.find(name);
        if (it != state->child_names.end()) return it->second;
        return {};
    }


    auto Node::children() const -> std::vector<ChildInfo> {
        std::lock_guard guard(detail::tree_mutex);
        std::vector<ChildInfo> result;
        result.reserve(state->children.size());
        for (auto* child : state->descendants()) result.push_back({child->name, child->id});
        return result;
    }


    auto Node::add_child(std::string name, UniqueObject child, Completion complete) -> void {
        detail::Edit edit{
            detail::EditKind::Add, id(), child.get_id(), std::move(name), std::move(complete), std::move(child)};
        if (!id().available()) {
            // Derived constructors may compose children before the pool publishes this node.
            state->construction_edits.push_back(std::move(edit));
            return;
        }
        edit.submit();
    }


    auto Node::detach_child(ObjectId child, Completion complete) -> void {
        detail::Edit{detail::EditKind::Detach, id(), child, {}, std::move(complete), {}}.submit();
    }


    auto Node::remove_child(ObjectId child, Completion complete) -> void {
        detail::Edit{detail::EditKind::Remove, id(), child, {}, std::move(complete), {}}.submit();
    }


    auto Node::rename(std::string name, Completion complete) -> void {
        detail::Edit{detail::EditKind::Rename, id(), {}, std::move(name), std::move(complete), {}}.submit();
    }


    auto Node::reparent(ObjectId parent, Completion complete) -> void {
        detail::Edit{detail::EditKind::Reparent, id(), parent, {}, std::move(complete), {}}.submit();
    }


    auto Node::post(std::function<void(Node&)> callback, std::function<void(NodeError)> complete) -> bool {
        if (!callback) throw std::invalid_argument("empty node callback");
        std::lock_guard guard(detail::tree_mutex);
        auto* runtime = state->runtime;
        if (!runtime || state->retired) return false;
        std::lock_guard queue_guard(runtime->queue_mutex);
        if (!runtime->accepting) return false;
        runtime->posts.push_back({id(), std::move(callback), std::move(complete)});
        return true;
    }
} // namespace gkit::core
