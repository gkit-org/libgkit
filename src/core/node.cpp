/** @file node.cpp
 * @brief Node ownership, immutable child snapshots and atomic structural transactions.
 */
#include "gkit/core/node.hpp"

#include "core/node_internal.hpp"
#include "gkit/core/reflect/registry.hpp"

#include <algorithm>
#include <unordered_set>

namespace gkit::core::detail {
    std::mutex tree_mutex;
    thread_local NodeState* execution_domain      = nullptr;
    thread_local TreeRuntime* coordinator_runtime = nullptr;

    /** @brief Bind a newly registered Node to its separately owned tree record, before publishing any handle. */
    auto make_node(UniqueObject object) -> std::shared_ptr<NodeState> {
        auto state          = std::make_shared<NodeState>(std::move(object));
        state->node().state = state;
        return state;
    }
    /**
     * @brief Retire an owned subtree iteratively, including descendants of externally pinned Node objects.
     * @note Detaches ownership under tree_mutex, then releases pool registrations outside the lock so user
     * destructors cannot deadlock membership inspection. Does not invoke virtual lifecycle callbacks.
     */
    auto destroy_node(std::shared_ptr<NodeState> state) noexcept -> void {
        if (!state) return;
        // No ownership is stored in Node itself: pinning a parent cannot keep child IDs registered.
        std::vector<UniqueObject> objects;
        {
            std::lock_guard lock(tree_mutex);
            std::vector<std::shared_ptr<NodeState>> pending{std::move(state)};
            while (!pending.empty()) {
                auto current = std::move(pending.back());
                pending.pop_back();
                for (auto& child : current->children) pending.push_back(std::move(child));
                current->children.clear();
                current->parent.reset();
                current->runtime.reset();
                if (current->owner) objects.push_back(std::move(*current->owner));
                current->owner.reset();
            }
        }
    }
    /**
     * @brief Check membership/domain permission and pin object lifetime before invoking user code without locks.
     * @return False when the record or registration expired; true after callback invocation.
     * @throws std::logic_error On access outside the owning domain/thread.
     */
    auto access_node(const std::weak_ptr<NodeState>& weak, const std::function<void(Node&)>& callback) -> bool {
        std::shared_ptr<Node> pinned;
        {
            std::lock_guard lock(tree_mutex);
            auto state = weak.lock();
            if (!state || !state->owner) return false;
            if (state->runtime && state->runtime->active) {
                if (coordinator_runtime != state->runtime.get() &&
                    (!execution_domain || state->domain != execution_domain))
                    throw std::logic_error("cross-domain node access; use post() or a published snapshot");
            } else if (state->thread != std::this_thread::get_id()) {
                throw std::logic_error("detached/stopped tree belongs to its creating thread");
            }
            pinned = state->id.lock<Node>();
        }
        if (!pinned) return false;
        callback(*pinned);
        return true;
    }
    auto node_id(const std::weak_ptr<NodeState>& weak) -> ObjectId {
        auto state = weak.lock();
        return state ? state->id : ObjectId{};
    }
    auto node_children(const std::weak_ptr<NodeState>& weak) -> ChildrenSnapshot {
        auto state = weak.lock();
        return state ? *state->snapshot.load() : ChildrenSnapshot{};
    }
    /**
     * @brief Build immutable ordered metadata and hash indexes before a transaction publishes any topology.
     * @param children Final ordered records for one parent.
     * @param names Staged name overrides; other records retain their current names.
     * @throws std::invalid_argument On empty/duplicate sibling names. Allocation errors leave the live tree unchanged.
     */
    auto NodeState::build_snapshot(const std::vector<std::shared_ptr<NodeState>>& children,
                                   const std::unordered_map<NodeState*, std::string>& names)
        -> std::shared_ptr<const ChildrenSnapshot> {
        auto data = std::make_shared<ChildrenSnapshot::Data>();
        data->entries.reserve(children.size());
        for (const auto& child : children) {
            auto found = names.find(child.get());
            auto name  = found == names.end() ? child->name : found->second;
            if (name.empty() || !data->names.emplace(name, data->entries.size()).second)
                throw std::invalid_argument("empty or duplicate child name");
            data->ids.emplace(child->id, data->entries.size());
            data->entries.push_back({std::move(name), NodeHandle<>(child)});
        }
        return std::shared_ptr<const ChildrenSnapshot>(new ChildrenSnapshot(std::move(data)));
    }
    auto assign_runtime(const std::shared_ptr<NodeState>& root, const std::shared_ptr<TreeRuntime>& runtime) -> void {
        // Parent links allow a depth-first walk with constant auxiliary storage.
        auto current          = root;
        root->traversal_index = 0;
        while (current) {
            current->runtime = runtime;
            current->thread  = std::this_thread::get_id();
            if (current->traversal_index < current->children.size()) {
                current                  = current->children[current->traversal_index++];
                current->traversal_index = 0;
            } else {
                if (current == root) break;
                current = current->parent.lock();
            }
        }
    }

    auto enter_nodes(const std::shared_ptr<NodeState>& root) -> void {
        std::vector<std::pair<std::shared_ptr<NodeState>, bool>> pending{{root, false}};
        while (!pending.empty()) {
            auto [current, after] = std::move(pending.back());
            pending.pop_back();
            if (after) {
                if (!current->prepared) {
                    current->prepared = true;
                    current->node().ready();
                }
                continue;
            }
            if (current->entered) continue;
            current->entered = true;
            current->node().enter_tree();
            pending.push_back({current, true});
            for (auto it = current->children.rbegin(); it != current->children.rend(); ++it)
                pending.push_back({*it, false});
        }
    }
    auto exit_nodes(const std::shared_ptr<NodeState>& root, std::exception_ptr& failure) noexcept -> void {
        auto current          = root;
        root->traversal_index = 0;
        while (current) {
            if (current->traversal_index < current->children.size()) {
                current                  = current->children[current->traversal_index++];
                current->traversal_index = 0;
                continue;
            }
            if (current->entered) {
                current->entered = false;
                try {
                    current->node().exit_tree();
                } catch (...) {
                    if (!failure) failure = std::current_exception();
                }
            }
            if (current == root) break;
            current = current->parent.lock();
        }
    }
    /**
     * @brief Acquire the target runtime safely, then enqueue without acquiring any user-state lock.
     * @return False for an expired/detached target or a closed runtime, with no completion callback invocation.
     * @note Destruction after acceptance is detected when the coordinator drains the weak target.
     */
    auto post_node(const std::weak_ptr<NodeState>& weak,
                   std::function<void(Node&)> callback,
                   std::function<void(NodeError)> complete) -> bool {
        std::shared_ptr<TreeRuntime> runtime;
        {
            std::lock_guard lock(tree_mutex);
            auto state = weak.lock();
            if (state && state->owner) runtime = state->runtime;
        }
        if (!runtime) return false;
        std::lock_guard lock(runtime->queue_mutex);
        if (!runtime->accepting) return false;
        runtime->posts.push_back({weak, std::move(callback), std::move(complete)});
        return true;
    }
    auto drain_edits(const std::shared_ptr<TreeRuntime>& runtime, bool cancel) -> void {
        std::deque<std::function<void(bool)>> edits;
        {
            std::lock_guard lock(runtime->queue_mutex);
            edits.swap(runtime->edits);
        }
        std::exception_ptr failure;
        for (auto& edit : edits) {
            try {
                edit(cancel || bool(failure));
            } catch (...) {
                if (!failure) failure = std::current_exception();
            }
        }
        if (failure) std::rethrow_exception(failure);
    }
    auto drain_posts(const std::shared_ptr<TreeRuntime>& runtime, bool cancel) -> void {
        std::deque<TreeRuntime::Post> posts;
        {
            std::lock_guard lock(runtime->queue_mutex);
            posts.swap(runtime->posts);
        }
        std::exception_ptr failure;
        for (auto& post : posts) {
            auto error = NodeError::Stopped;
            try {
                if (!cancel && !failure) {
                    bool belongs = false;
                    {
                        std::lock_guard guard(tree_mutex);
                        auto target = post.target.lock();
                        belongs     = target && target->runtime == runtime;
                    }
                    error = belongs && access_node(post.target, post.callback) ? NodeError::None : NodeError::Expired;
                }
            } catch (...) {
                error = NodeError::CallbackFailed;
                if (!failure) failure = std::current_exception();
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
    /**
     * @brief Publish Node reflection once, before registering derived node types.
     * @note The name property routes through NodeEditor rather than writing the indexed field directly.
     */
    Node::Node() {
        static const bool registered = [] {
            auto& db = reflect::ClassDB::instance();
            db.register_class<Node>("Node", [](auto& staging) {
                staging.template add_property<Node>(
                    "Node",
                    "name",
                    [](const Node& node) {
                        return Value(node.name());
                    },
                    [](Node& node, const Value& value) {
                        auto name = value.as_string();
                        node.edit_children([&](NodeEditor& edit) {
                            edit.rename(node.handle(), name);
                        });
                    });
            });
            return true;
        }();
        (void)registered;
    }
    auto ChildrenSnapshot::at(std::size_t index) const -> std::optional<NodeHandle<>> {
        if (index >= size()) return std::nullopt;
        return data->entries[index].node;
    }
    auto ChildrenSnapshot::find(const std::string& name) const -> std::optional<NodeHandle<>> {
        auto it = data->names.find(name);
        return it == data->names.end() ? std::nullopt : at(it->second);
    }
    auto ChildrenSnapshot::find(ObjectId id) const -> std::optional<NodeHandle<>> {
        auto it = data->ids.find(id);
        return it == data->ids.end() ? std::nullopt : at(it->second);
    }
    auto Node::handle() const -> NodeHandle<> {
        auto value = state.lock();
        if (!value) throw std::logic_error("create nodes with NodeOwner::create");
        return NodeHandle<>(value);
    }
    auto Node::parent() const -> std::optional<NodeHandle<>> {
        auto value = state.lock();
        if (!value) return std::nullopt;
        auto parent = value->parent.lock();
        return parent ? std::optional(NodeHandle<>(parent)) : std::nullopt;
    }
    auto Node::name() const -> std::string {
        auto value = state.lock();
        return value ? value->name : std::string{};
    }
    auto Node::children() const -> ChildrenSnapshot {
        return handle().children();
    }
    auto Node::set_parallel(bool value) -> void {
        auto record = state.lock();
        if (record && record->runtime && record->runtime->active)
            throw std::logic_error("parallel domains cannot change during run");
        parallel = value;
    }

    /** @brief Move-owning command storage; weak targets prevent queued operations from retaining removed trees. */
    struct NodeEditor::Impl {
        enum class Kind {
            Add,
            Destroy,
            Detach,
            Rename,
            Reparent
        };
        struct Command {
            Kind kind;
            NodeHandle<> node;
            NodeHandle<> parent;
            std::string name;
            std::optional<NodeOwner<>> owner;
        };
        std::weak_ptr<detail::NodeState> anchor;
        std::weak_ptr<detail::TreeRuntime> submitted_runtime;
        bool queued = false;
        std::vector<Command> commands;
        auto apply(bool cancel, const std::function<void(EditResult)>& complete) -> void;
    };
    NodeEditor::NodeEditor(std::shared_ptr<detail::NodeState> anchor) : impl(std::make_unique<Impl>()) {
        impl->anchor = anchor;
    }
    NodeEditor::~NodeEditor() = default;
    auto NodeEditor::add(NodeHandle<> parent, std::string name, NodeOwner<> child) -> void {
        if (!child.state) throw std::invalid_argument("empty child owner");
        auto handle = child.handle();
        impl->commands.push_back({Impl::Kind::Add, handle, parent, std::move(name), std::move(child)});
    }
    auto NodeEditor::destroy(NodeHandle<> node) -> void {
        impl->commands.push_back({Impl::Kind::Destroy, node, {}, {}, {}});
    }
    auto NodeEditor::detach(NodeHandle<> node) -> void {
        impl->commands.push_back({Impl::Kind::Detach, node, {}, {}, {}});
    }
    auto NodeEditor::rename(NodeHandle<> node, std::string name) -> void {
        impl->commands.push_back({Impl::Kind::Rename, node, {}, std::move(name), {}});
    }
    auto NodeEditor::reparent(NodeHandle<> node, NodeHandle<> parent) -> void {
        impl->commands.push_back({Impl::Kind::Reparent, node, parent, {}, {}});
    }
    /**
     * @brief Validate the final graph, publish a whole structural batch, then dispatch lifecycle/completion callbacks.
     * @param cancel Return staged owners with cancellation status instead of modifying topology.
     * @param complete Optional receiver of detached/rejected subtree ownership.
     * @note Copies only affected sibling lists. Each list is compacted once; indexes/snapshots are allocated before
     * publication. Repeated structural edits, removal overlaps, cross-tree destinations and cycles are rejected.
     * Validation failure changes no live records. User callbacks run after releasing tree_mutex; a callback failure
     * cannot roll back publication and instead propagates to Processer cleanup.
     */
    auto NodeEditor::Impl::apply(bool cancel, const std::function<void(EditResult)>& complete) -> void {
        using State = detail::NodeState;
        EditResult result;
        std::vector<std::pair<std::shared_ptr<State>, Kind>> removed;
        std::vector<std::shared_ptr<State>> added;
        auto origin  = anchor.lock();
        auto runtime = origin ? origin->runtime : nullptr;
        result.error = cancel ? NodeError::Stopped : NodeError::None;
        if (!origin || !origin->owner || (queued && runtime != submitted_runtime.lock()))
            result.error = NodeError::Expired;
        if (result.error == NodeError::None) {
            try {
                std::lock_guard guard(detail::tree_mutex);
                auto root = origin;
                while (auto parent = root->parent.lock()) root = parent;
                std::unordered_map<State*, std::vector<std::shared_ptr<State>>> lists;
                std::unordered_map<State*, std::shared_ptr<State>> parents;
                std::unordered_map<State*, std::string> names;
                std::unordered_set<State*> changed;
                std::unordered_set<State*> outgoing;
                auto require = [&](NodeHandle<> handle) {
                    auto record = handle.state.lock();
                    if (!record || !record->owner) throw std::invalid_argument("expired node");
                    auto top = record;
                    while (auto parent = top->parent.lock()) top = parent;
                    if (top != root) throw std::invalid_argument("node belongs to another tree");
                    return record;
                };
                auto list = [&](const std::shared_ptr<State>& parent) -> auto& {
                    auto [it, inserted] = lists.try_emplace(parent.get());
                    if (inserted) it->second = parent->children;
                    return it->second;
                };
                // Each node has at most one structural operation per batch; renames may accompany it.
                for (auto& command : commands) {
                    auto node = command.kind == Kind::Add ? command.owner->state : require(command.node);
                    if (command.kind == Kind::Rename) {
                        names[node.get()] = command.name;
                        if (auto parent = node->parent.lock()) list(parent);
                        continue;
                    }
                    if (!changed.insert(node.get()).second) throw std::invalid_argument("conflicting edits");
                    if (command.kind != Kind::Add) {
                        auto parent = node->parent.lock();
                        if (!parent) throw std::invalid_argument("cannot remove or move tree root");
                        // Compact once per affected parent below, not once per command.
                        list(parent);
                    }
                    if (command.kind == Kind::Add || command.kind == Kind::Reparent) {
                        auto parent         = require(command.parent);
                        parents[node.get()] = parent;
                        list(parent);
                        if (command.kind == Kind::Add) {
                            if (!node->parent.expired() || node->runtime)
                                throw std::invalid_argument("child already attached");
                            names[node.get()] = command.name;
                            added.push_back(node);
                        }
                    } else {
                        parents[node.get()] = {};
                        outgoing.insert(node.get());
                        removed.push_back({node, command.kind});
                    }
                }
                for (auto& [parent, children] : lists)
                    std::erase_if(children, [&](const auto& child) {
                        return changed.contains(child.get());
                    });
                for (auto& command : commands) {
                    if (command.kind == Kind::Add || command.kind == Kind::Reparent) {
                        auto node = command.node.state.lock();
                        lists.at(parents.at(node.get()).get()).push_back(node);
                    }
                }
                // Reject overlapping removal/edit operations, and validate the final parent graph.
                for (auto& command : commands) {
                    auto node     = command.node.state.lock();
                    auto ancestor = node->parent.lock();
                    while (ancestor) {
                        if (outgoing.contains(ancestor.get()))
                            throw std::invalid_argument("edit inside removed subtree");
                        ancestor = ancestor->parent.lock();
                    }
                    if (command.kind == Kind::Rename && outgoing.contains(node.get()))
                        throw std::invalid_argument("rename of removed node");
                }
                for (auto& [node, parent] : parents) {
                    std::unordered_set<State*> path{node};
                    auto current = parent;
                    while (current) {
                        if (!path.insert(current.get()).second) throw std::invalid_argument("tree cycle");
                        if (outgoing.contains(current.get())) throw std::invalid_argument("destination is removed");
                        auto it = parents.find(current.get());
                        current = it == parents.end() ? current->parent.lock() : it->second;
                    }
                }
                std::unordered_map<State*, std::shared_ptr<const ChildrenSnapshot>> snapshots;
                for (const auto& [parent, children] : lists)
                    snapshots.emplace(parent, State::build_snapshot(children, names));
                result.owners.reserve(removed.size());
                // All validation and snapshot allocations precede publication.
                for (auto& [node, name] : names) node->name.swap(name);
                for (auto& [node, parent] : parents) node->parent = parent;
                for (auto& [parent, children] : lists) {
                    parent->children.swap(children);
                    parent->snapshot.store(snapshots.at(parent));
                }
                for (auto& command : commands)
                    if (command.owner) command.owner->state.reset();
                for (const auto& node : added) detail::assign_runtime(node, runtime);
            } catch (const std::invalid_argument&) {
                result.error = NodeError::InvalidEdit;
                removed.clear();
                added.clear();
            }
        }
        if (result.error != NodeError::None) {
            for (auto& command : commands) {
                if (command.owner) {
                    {
                        std::lock_guard guard(detail::tree_mutex);
                        detail::assign_runtime(command.owner->state, {});
                    }
                    result.owners.push_back(std::move(*command.owner));
                }
            }
        }
        std::exception_ptr failure;
        for (auto& [node, kind] : removed) {
            detail::exit_nodes(node, failure);
            {
                std::lock_guard guard(detail::tree_mutex);
                detail::assign_runtime(node, {});
            }
            if (kind == Kind::Detach)
                result.owners.push_back(NodeOwner<>(node));
            else
                detail::destroy_node(node);
        }
        if (!failure && runtime && runtime->active) {
            try {
                for (const auto& node : added) detail::enter_nodes(node);
            } catch (...) {
                failure = std::current_exception();
            }
        }
        try {
            if (complete) complete(std::move(result));
        } catch (...) {
            if (!failure) failure = std::current_exception();
        }
        if (failure) std::rethrow_exception(failure);
        if (!complete && result.error != NodeError::None && result.error != NodeError::Stopped)
            throw std::invalid_argument("node edit rejected");
    }
    /**
     * @brief Transfer the batch into the runtime queue, or apply synchronously for a stopped/detached tree.
     * @param complete Completion callback moved into the queued command when running.
     * @note Never waits for a frame boundary. A closed running runtime rejects new submissions.
     */
    auto NodeEditor::submit(std::function<void(EditResult)> complete) -> void {
        auto origin = impl->anchor.lock();
        auto batch  = std::shared_ptr<Impl>(std::move(impl));
        if (origin && origin->runtime && origin->runtime->active) {
            auto runtime             = origin->runtime;
            batch->submitted_runtime = runtime;
            batch->queued            = true;
            std::lock_guard lock(runtime->queue_mutex);
            if (!runtime->accepting) throw std::logic_error("tree is stopping");
            runtime->edits.push_back([batch, complete = std::move(complete)](bool cancel) {
                batch->apply(cancel, complete);
            });
        } else
            batch->apply(false, complete);
    }
    auto Node::edit_children(const std::function<void(NodeEditor&)>& edit, std::function<void(EditResult)> complete)
        -> void {
        auto record = state.lock();
        if (!record) throw std::logic_error("unowned node");
        NodeEditor editor(record);
        edit(editor);
        editor.submit(std::move(complete));
    }
} // namespace gkit::core
