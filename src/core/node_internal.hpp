/** @file node_internal.hpp
 * @brief Private tree records and scheduler coordination; not a public extension API.
 */
#pragma once

#include "gkit/core/node.hpp"

#include <deque>
#include <thread>

namespace gkit::core::detail {
    // Protects membership inspection only; never held across user callbacks.
    extern std::mutex tree_mutex;
    extern thread_local NodeState* execution_domain;
    extern thread_local TreeRuntime* coordinator_runtime;

    /**
     * @brief Shared membership/queue state for one Processer, without owning the tree.
     * @note active is protected by tree_mutex. Queues and accepting are protected by queue_mutex.
     * Queue drains swap work into local storage and release locks before invoking user code.
     */
    struct TreeRuntime {
        std::thread::id thread = std::this_thread::get_id();
        bool active            = false; // guarded by tree_mutex
        std::mutex queue_mutex;
        bool accepting = true;
        std::deque<std::function<void(bool)>> edits;
        struct Post {
            std::weak_ptr<NodeState> target;
            std::function<void(Node&)> callback;
            std::function<void(NodeError)> complete;
        };
        std::deque<Post> posts;
    };

    /**
     * @brief Tree-owned record, deliberately separate from the lifetime-pinned Node object.
     * @note Parents own child records; parent links and public handles are weak. Only records own UniqueObject
     * registrations, so a retained ObjectId::lock() borrow of a Node cannot retain its descendant registrations.
     * Topology changes occur only on the coordinator/stopped owner thread. External readers use snapshot exclusively.
     */
    struct NodeState : std::enable_shared_from_this<NodeState> {
        std::optional<UniqueObject> owner;
        ObjectId id;
        std::weak_ptr<NodeState> parent;
        std::vector<std::shared_ptr<NodeState>> children;
        std::string name;
        std::shared_ptr<TreeRuntime> runtime;
        std::thread::id thread      = std::this_thread::get_id();
        NodeState* domain           = nullptr;
        std::size_t traversal_index = 0;
        bool entered                = false;
        bool prepared               = false;
        std::atomic<std::shared_ptr<const ChildrenSnapshot>> snapshot;
        explicit NodeState(UniqueObject value) : owner(std::move(value)), id(owner->get_id()) {
            snapshot.store(std::make_shared<ChildrenSnapshot>());
        }
        auto node() -> Node& { return *static_cast<Node*>(owner->get()); }
        static auto build_snapshot(const std::vector<std::shared_ptr<NodeState>>& children,
                                   const std::unordered_map<NodeState*, std::string>& names = {})
            -> std::shared_ptr<const ChildrenSnapshot>;
    };
    /**
     * @brief Temporarily authorize access to one execution domain or an idle tree's coordinator phase.
     * @note Saves/restores thread-local permissions on exception paths. Coordinator access is runtime-specific,
     * so entering a different Processer cannot grant access to another concurrently updating tree.
     */
    struct AccessScope {
        NodeState* previous               = execution_domain;
        TreeRuntime* previous_coordinator = coordinator_runtime;
        explicit AccessScope(NodeState* domain = nullptr, TreeRuntime* coordinator = nullptr) {
            execution_domain    = domain;
            coordinator_runtime = coordinator;
        }
        ~AccessScope() {
            execution_domain    = previous;
            coordinator_runtime = previous_coordinator;
        }
    };
    /**
     * @brief Assign runtime and coordinator-thread affinity to every record in a subtree.
     * @param root Root of the affected subtree.
     * @param runtime Destination runtime, or null when detaching.
     * @note Requires exclusive topology access. Uses parent links and per-record cursors: O(n), no allocations.
     */
    auto assign_runtime(const std::shared_ptr<NodeState>& root, const std::shared_ptr<TreeRuntime>& runtime) -> void;
    /**
     * @brief Invoke parent-first entry and child-first one-time readiness on the coordinator.
     * @param root Subtree being attached to a running tree.
     * @note Marks entry before invoking user code so failure cleanup visits partially entered subtrees.
     * Structure edits from callbacks are queued. Exceptions propagate to the run cleanup boundary.
     */
    auto enter_nodes(const std::shared_ptr<NodeState>& root) -> void;
    /**
     * @brief Exit all entered descendants before their parent, without allocating traversal storage.
     * @param root Subtree leaving the running tree.
     * @param failure Receives the first callback exception only if no earlier failure was recorded.
     * @note Requires idle workers and stable topology. Never throws; continues cleanup after callback failures.
     */
    auto exit_nodes(const std::shared_ptr<NodeState>& root, std::exception_ptr& failure) noexcept -> void;
    /**
     * @brief Drain one captured edit queue on the coordinator, completing callbacks outside the queue mutex.
     * @param runtime Queue owner.
     * @param cancel Cancel every captured batch instead of committing it.
     * @note Callback submissions remain queued for a later frame. A failure cancels remaining captured batches
     * and is rethrown after their completion callbacks have run.
     */
    auto drain_edits(const std::shared_ptr<TreeRuntime>& runtime, bool cancel = false) -> void;
    /**
     * @brief Execute one captured post queue while all workers are idle.
     * @param runtime Target tree runtime.
     * @param cancel Complete captured requests with Stopped without invoking their node callbacks.
     * @note Expired targets complete with Expired. Callback failure cancels the remaining captured requests;
     * completion callbacks are still attempted, and the first exception is rethrown after draining.
     */
    auto drain_posts(const std::shared_ptr<TreeRuntime>& runtime, bool cancel = false) -> void;
} // namespace gkit::core::detail
