#pragma once

#include "gkit/core/node.hpp"

#include <deque>
#include <mutex>
#include <ranges>
#include <thread>
#include <unordered_map>

namespace gkit::core::detail {
    extern std::mutex tree_mutex;
    struct TreeRuntime;

    struct NodeState {
        explicit NodeState(Node& node) : node(node) {}
        Node& node;
        ObjectId id, parent;
        std::vector<UniqueObject> children;
        std::unordered_map<std::string, ObjectId> child_names;
        std::string name;
        TreeRuntime* runtime        = nullptr;
        std::size_t traversal_index = 0;
        bool entered = false, prepared = false, retired = false;

        static auto of(Node& node) -> NodeState& { return *node.state; }
        static auto resolve(ObjectId id) -> NodeState*;
        auto descendants() const {
            return children | std::views::transform([](const UniqueObject& owner) {
                       return static_cast<const Node*>(owner.get())->state.get();
                   });
        }
        auto find(ObjectId child) -> std::vector<UniqueObject>::iterator;
        auto append(const std::string& name, UniqueObject& child) -> bool;
        auto retire() noexcept -> void;
    };

    enum class EditKind {
        Add,
        Detach,
        Remove,
        Rename,
        Reparent
    };
    struct Edit {
        EditKind kind;
        ObjectId anchor, target;
        std::string name;
        Node::Completion complete;
        std::optional<UniqueObject> owner;
        auto submit() -> void;
        auto apply(TreeRuntime* runtime, std::unique_lock<std::mutex>& guard, bool cancel) -> void;
    };

    struct TreeRuntime {
        const std::thread::id thread = std::this_thread::get_id();
        bool active                  = false; // tree_mutex
        std::mutex queue_mutex;
        bool accepting = true;
        std::deque<Edit> edits;
        struct Post {
            ObjectId target;
            std::function<void(Node&)> callback;
            std::function<void(NodeError)> complete;
        };
        std::deque<Post> posts;
    };
    auto assign_runtime(NodeState* root, TreeRuntime* runtime) -> void;
    auto enter_nodes(NodeState* root) -> void;
    auto exit_nodes(NodeState* root, std::exception_ptr& failure) noexcept -> void;
    auto drain_edits(TreeRuntime& runtime, bool cancel = false) -> void;
    auto drain_posts(TreeRuntime& runtime, bool cancel = false) -> void;
} // namespace gkit::core::detail
