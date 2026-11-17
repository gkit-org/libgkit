/**
 * @file node.hpp
 * @brief Ownership, transactional tree composition, and execution-domain-safe node access.
 */
#pragma once

#include "gkit/core/input/input.hpp"
#include "gkit/core/unique_object.hpp"

#include <atomic>
#include <functional>
#include <optional>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

namespace gkit {
    class Processer;
}

namespace gkit::core {
    class Node;
    class NodeEditor;
    class ChildrenSnapshot;
    template<class T = Node>
    class NodeOwner;
    template<class T = Node>
    class NodeHandle;

    namespace detail {
        struct NodeState;
        struct TreeRuntime;
        auto make_node(UniqueObject object) -> std::shared_ptr<NodeState>;
        auto destroy_node(std::shared_ptr<NodeState> state) noexcept -> void;
        auto access_node(const std::weak_ptr<NodeState>& state, const std::function<void(Node&)>& callback) -> bool;
        auto node_id(const std::weak_ptr<NodeState>& state) -> ObjectId;
    } // namespace detail

    /**
     * @brief Timing and input shared by one update phase.
     * @note All callbacks in a phase observe the same values. Input edges belong to the input frame,
     * not to individual physics ticks; multiple fixed ticks may observe the same edge.
     */
    struct FrameContext {
        double delta               = 0; ///< Seconds: measured/clamped for process(), fixed for physics_process().
        std::uint64_t frame        = 0; ///< Zero-based logic-frame number, reset on each run().
        std::uint64_t physics_tick = 0; ///< Zero-based physics tick, or completed tick count during process().
        std::shared_ptr<const input::Snapshot>
            input; ///< Non-null during scheduled callbacks; immutable and retainable.
    };

    /** @brief Completion status for accepted edits and posted callbacks. */
    enum class NodeError {
        None, ///< The operation completed successfully.
        Expired, ///< The anchor/target expired or left its submitting runtime before execution.
        InvalidEdit, ///< Structural validation failed; no commands in the batch were applied.
        CallbackFailed, ///< The posted callback threw; run() also propagates the exception after cleanup.
        Stopped ///< The scheduler cancelled this operation during shutdown.
    };

    /**
     * @tparam T Node-derived type required by callbacks.
     * @brief Identity only. with() pins lifetime and executes in the owning domain, or throws on cross-domain access.
     * Missing objects/type mismatches return false. Do not retain callback references. Raw ObjectId::lock() is a
     * low-level escape hatch and does not synchronize fields. Stopped/detached trees belong to their creating or ownership-return thread.
     */
    template<class T>
    class NodeHandle {
        template<class>
        friend class NodeHandle;
        template<class>
        friend class NodeOwner;
        friend class Node;
        friend class NodeEditor;
        friend class gkit::Processer;
        friend struct detail::NodeState;
        std::weak_ptr<detail::NodeState> state;
        ObjectId identity;
        explicit NodeHandle(const std::shared_ptr<detail::NodeState>& value) :
            state(value), identity(detail::node_id(value)) {}

    public:
        /** @brief Create an unavailable handle; with() returns false and children() is empty. */
        NodeHandle() = default;
        /** @brief Widen a typed handle to a base-node handle without changing identity or ownership. */
        template<class U>
            requires std::derived_from<U, T>
        NodeHandle(const NodeHandle<U>& other) : state(other.state), identity(other.identity) {}
        /** @brief Return the stable generation-bearing identity, including after destruction. */
        auto id() const -> ObjectId { return identity; }
        /**
         * @brief Acquire child metadata from any thread.
         * @return The most recently published snapshot, or an empty snapshot for an expired handle.
         * @note Does not pin any node's lifetime. Unchanged child lists reuse their published storage.
         */
        auto children() const -> ChildrenSnapshot;
        /**
         * @brief Invoke a callback with a non-null, lifetime-pinned node reference.
         * @tparam Fn Callable accepting T&; any return value is discarded.
         * @param callback Operation to execute immediately, without a tree lock held.
         * @return True if invoked; false if the handle expired or the dynamic type does not match T.
         * @throws std::logic_error If the current thread/execution domain cannot access this node.
         * @note Callback exceptions propagate. References must not escape the callback. Use post() across domains.
         */
        template<class Fn>
        auto with(Fn&& callback) const -> bool {
            bool matched = false;
            detail::access_node(state, [&](Node& node) {
                if (auto* typed = dynamic_cast<T*>(&node)) {
                    matched = true;
                    std::invoke(callback, *typed);
                }
            });
            return matched;
        }
        /**
         * @brief Queue for the coordinator's exclusive pre-update phase. Accepted requests complete exactly once,
         * including expiry/cancellation; false means not accepted (no completion). A detached target cannot receive posts.
         * @param callback Operation receiving T& while all tree update workers are idle.
         * @param complete Optional status callback on the coordinator; must not retain node references.
         * @return Whether the request was accepted. Thread-safe, including during updates.
         * @throws std::invalid_argument If callback is empty.
         * @note Callback exceptions produce CallbackFailed, stop the run, and are rethrown after cleanup.
         * Requests posted while draining the queue are deferred to the next frame; shutdown cancels pending requests.
         */
        auto post(std::function<void(T&)> callback, std::function<void(NodeError)> complete = {}) const -> bool;
    };

    /** @brief One immutable child-list entry. The name is captured at snapshot publication. */
    struct ChildInfo {
        std::string name; ///< Name at publication, unaffected by subsequent renames.
        NodeHandle<> node; ///< Stable identity; may expire independently of this entry.
    };

    /**
     * @brief Immutable child metadata in insertion order, with O(1) indexed and average O(1) hashed lookup.
     * @note Copies share storage and are safe to read concurrently. Iterators remain valid while a copy owns that
     * storage. Indices belong only to this snapshot; retain a handle for later edits. A snapshot does not own children.
     */
    class ChildrenSnapshot {
        friend class Node;
        template<class>
        friend class NodeHandle;
        friend struct detail::NodeState;
        struct Data {
            std::vector<ChildInfo> entries;
            std::unordered_map<std::string, std::size_t> names;
            std::unordered_map<ObjectId, std::size_t> ids;
        };
        std::shared_ptr<const Data> data;
        explicit ChildrenSnapshot(std::shared_ptr<const Data> value) : data(std::move(value)) {}

    public:
        /** @brief Create an empty child-list snapshot. */
        ChildrenSnapshot() : data(std::make_shared<Data>()) {}
        /** @brief Return the number of entries at publication time. */
        auto size() const -> std::size_t { return data->entries.size(); }
        /** @brief Return whether this snapshot contains no entries. */
        auto empty() const -> bool { return data->entries.empty(); }
        /** @brief Return the first immutable entry iterator. */
        auto begin() const { return data->entries.begin(); }
        /** @brief Return the iterator past the last immutable entry. */
        auto end() const { return data->entries.end(); }
        /** @brief Return the first reverse iterator, starting at the last child. */
        auto rbegin() const { return data->entries.rbegin(); }
        /** @brief Return the reverse iterator past the first child. */
        auto rend() const { return data->entries.rend(); }
        /**
         * @brief Find the identity at a snapshot-local index.
         * @param index Zero-based position in this snapshot, not in a later version of the tree.
         * @return A handle, or std::nullopt if out of range. A returned handle may already be expired.
         */
        auto at(std::size_t index) const -> std::optional<NodeHandle<>>;
        /** @brief Look up the exact captured sibling name; return std::nullopt when absent. */
        auto find(const std::string& name) const -> std::optional<NodeHandle<>>;
        /** @brief Look up a generation-bearing identity; return std::nullopt when absent. */
        auto find(ObjectId id) const -> std::optional<NodeHandle<>>;
    };

    /**
     * @brief Exclusive ownership of a detached subtree, including all descendant registrations.
     * @tparam T Node-derived root type.
     * @note Create through create(), transfer into NodeEditor::add(), or receive from NodeEditor::detach().
     * @code
     * auto tree = NodeOwner<Node>::create();
     * tree.with([](Node& root) {
     *     root.edit_children([&](NodeEditor& edit) {
     *         edit.emplace<Node>(root.handle(), "player");
     *         edit.emplace<Node>(root.handle(), "enemies");
     *     });
     * });
     * @endcode
     * Moving does not change IDs. A moved-from owner may only be destroyed or assigned a new owner.
     * Detached nodes are accessed on their creating/ownership-return thread; moving an owner does not transfer thread affinity.
     */
    template<class T>
    class NodeOwner {
        template<class>
        friend class NodeOwner;
        friend class NodeEditor;
        friend class gkit::Processer;
        std::shared_ptr<detail::NodeState> state;
        explicit NodeOwner(std::shared_ptr<detail::NodeState> value) : state(std::move(value)) {}

    public:
        /** @brief Retire the subtree's IDs; existing low-level lifetime pins may delay physical destruction. */
        ~NodeOwner() { detail::destroy_node(std::move(state)); }
        NodeOwner(const NodeOwner&)                    = delete;
        auto operator=(const NodeOwner&) -> NodeOwner& = delete;
        /** @brief Transfer the entire subtree and leave other empty. */
        NodeOwner(NodeOwner&& other) noexcept : state(std::move(other.state)) {}
        /** @brief Retire this owner's old subtree and take other's ownership; self-move is harmless. */
        auto operator=(NodeOwner&& other) noexcept -> NodeOwner& {
            if (this != &other) {
                detail::destroy_node(std::move(state));
                state = std::move(other.state);
            }
            return *this;
        }
        /** @brief Transfer a derived-root owner into a base-root owner without changing the dynamic type. */
        template<class U>
            requires std::derived_from<U, T>
        NodeOwner(NodeOwner<U>&& other) noexcept : state(std::move(other.state)) {}
        /**
         * @brief Construct and register a detached subtree root with no parent or lifecycle callbacks.
         * @tparam Args Constructor argument types for T.
         * @param args Arguments forwarded to T's constructor.
         * @return A non-empty exclusive owner. Construction/allocation failures propagate as exceptions.
         */
        template<class... Args>
        static auto create(Args&&... args) -> NodeOwner {
            static_assert(std::derived_from<T, Node>);
            return NodeOwner(detail::make_node(UniqueObject::create<T>(std::forward<Args>(args)...)));
        }
        /**
         * @brief Obtain a non-owning typed identity.
         * @throws std::logic_error If this owner has already transferred its subtree.
         */
        auto handle() const -> NodeHandle<T> {
            if (!state) throw std::logic_error("moved-from NodeOwner");
            return NodeHandle<T>(state);
        }
        /** @brief Access the root through NodeHandle::with(), with the same lifetime and domain rules. */
        template<class Fn>
        auto with(Fn&& callback) const -> bool {
            return handle().with(std::forward<Fn>(callback));
        }
    };

    /**
     * @brief Outcome of one structural batch, delivered after publication and lifecycle callbacks.
     * @note Structural validation is atomic; exceptions from lifecycle/completion callbacks cannot undo a committed
     * batch and instead terminate run() with cleanup. Keep returned owners to preserve detached/rejected subtrees; their affinity becomes the completion thread.
     */
    struct EditResult {
        NodeError error = NodeError::None; ///< Validation or cancellation result.
        /// Successful detachments, or unconsumed additions on failure/cancellation, in command order.
        std::vector<NodeOwner<>> owners;
    };

    /**
     * @brief Transactional structural edits. Existing nodes have at most one structural operation per batch.
     * Names are validated against the final sibling lists. Destinations must exist before the batch.
     * Running trees commit next frame; stopped/detached trees commit immediately. Without a completion callback,
     * rejected edits throw; pending edits at shutdown are discarded. To recover additions on failure, supply completion.
     */
    class NodeEditor {
        friend class Node;
        struct Impl;
        std::unique_ptr<Impl> impl;
        explicit NodeEditor(std::shared_ptr<detail::NodeState> anchor);
        auto submit(std::function<void(EditResult)> complete) -> void;

    public:
        /** @brief Discard unsubmitted commands and their staged subtree owners. */
        ~NodeEditor();
        NodeEditor(const NodeEditor&)                    = delete;
        auto operator=(const NodeEditor&) -> NodeEditor& = delete;
        /**
         * @brief Stage a detached subtree for insertion at the end of a parent's child list.
         * @param parent Existing destination in the same tree as the editor anchor.
         * @param name Non-empty name, unique among the destination's final siblings.
         * @param child Exclusive ownership transferred into the batch immediately.
         * @throws std::invalid_argument If child is moved-from.
         * @note On validation failure ownership is returned in EditResult::owners when completion is supplied.
         */
        auto add(NodeHandle<> parent, std::string name, NodeOwner<> child) -> void;
        /**
         * @brief Construct and stage a child using add().
         * @tparam T Concrete Node-derived type.
         * @param parent Destination parent.
         * @param name Final sibling name.
         * @param args Arguments forwarded to T's constructor.
         * @return A typed identity for the staged node; this is not a promise that the batch will commit.
         */
        template<class T = Node, class... Args>
        auto emplace(NodeHandle<> parent, std::string name, Args&&... args) -> NodeHandle<T> {
            auto child  = NodeOwner<T>::create(std::forward<Args>(args)...);
            auto handle = child.handle();
            add(parent, std::move(name), std::move(child));
            return handle;
        }
        /**
         * @brief Stage removal and retirement of a complete subtree.
         * @param node Existing non-root node. All descendant IDs expire when the command commits.
         * @note In-flight frame callbacks finish first; entered nodes receive child-first exit_tree().
         */
        auto destroy(NodeHandle<> node) -> void;
        /**
         * @brief Stage removal without destroying the subtree.
         * @param node Existing non-root node whose ID and application state must survive.
         * @note The completion result owns the detached tree. Without completion it is destroyed when the result dies.
         * Reattachment calls enter_tree() again, but does not repeat a previously invoked ready().
         */
        auto detach(NodeHandle<> node) -> void;
        /**
         * @brief Stage a name change and rebuild affected name indexes atomically.
         * @param node Existing node; a root's name is only a label, not a sibling lookup key.
         * @param name New name. Attached siblings must have distinct, non-empty names.
         */
        auto rename(NodeHandle<> node, std::string name) -> void;
        /**
         * @brief Move an existing subtree to the end of another parent's list without lifecycle re-entry.
         * @param node Non-root subtree to move, preserving IDs and application state.
         * @param parent Existing destination in the same tree; cycles and removed destinations are rejected.
         * @note Same-parent moves place the node last. Cross-Processer moves require detach() followed by add().
         */
        auto reparent(NodeHandle<> node, NodeHandle<> parent) -> void;
    };

    /**
     * @brief Basic composable tree element, independent of the scene and graphic modules.
     * @note Construct managed nodes through NodeOwner::create(). All live-node methods require a valid owning-domain
     * callback, lifecycle callback, or stopped-tree access on its creating thread. NodeHandle::children() and post()
     * are the cross-thread interfaces. Never retain raw references beyond their callback.
     * @note Lifecycle callbacks run on the coordinator without structure locks. Updates are parent-first;
     * independent parallel subtrees have no relative ordering. Structural edits never occur during update traversal.
     */
    class Node : public Object {
        friend auto detail::make_node(UniqueObject) -> std::shared_ptr<detail::NodeState>;
        std::weak_ptr<detail::NodeState> state;
        bool enabled  = true;
        bool parallel = false;

    public:
        /** @brief Initialize node defaults and lazily publish the Node reflection description. */
        Node();
        Node(const Node&)                    = delete;
        auto operator=(const Node&) -> Node& = delete;
        /** @brief Destroy fields only; lifecycle exit is performed by the scheduler before ownership is released. */
        ~Node() override = default;
        /** @brief Return this node's identity; throws std::logic_error if not created through NodeOwner. */
        auto handle() const -> NodeHandle<>;
        /** @brief Return a non-owning parent handle, or std::nullopt for a detached/processor root. */
        auto parent() const -> std::optional<NodeHandle<>>;
        /** @brief Copy the current name; change it through NodeEditor::rename() to preserve indexes. */
        auto name() const -> std::string;
        /** @brief Obtain the published child metadata; see NodeHandle::children() for cross-thread access. */
        auto children() const -> ChildrenSnapshot;
        /**
         * @brief Describe one atomic batch of structural operations.
         * @param edit Immediate callback receiving a temporary editor; it must not retain the editor.
         * @param complete Optional callback receiving status and returned owners after commit/cancellation.
         * @note While running, submission is non-blocking and commits at the next frame boundary. Otherwise it commits
         * synchronously. Do not wait inside updates for a deferred result. Callbacks may submit further batches.
         * @throws std::invalid_argument On rejected edits without a completion callback.
         * @throws std::logic_error If the node is unowned or a running tree has closed submissions.
         * @note Construction, user-callback and allocation exceptions propagate; they do not imply rollback of a
         * batch that has already been published. Validation failure itself never partially applies a batch.
         */
        auto edit_children(const std::function<void(NodeEditor&)>& edit, std::function<void(EditResult)> complete = {})
            -> void;
        /**
         * @brief Access a direct child by current index, name, or ObjectId.
         * @tparam T Required child type, defaulting to Node.
         * @param key Index in the snapshot acquired by this call, exact name, or stable ID.
         * @param callback Callable receiving T&; its return value is discarded.
         * @return Whether a matching live child was visited.
         * @note Same execution-domain rules as NodeHandle::with(). For deferred index operations, resolve an explicit
         * ChildrenSnapshot::at() first and submit the resulting identity rather than the index.
         */
        template<class T = Node, class Key, class Fn>
        auto with_child(const Key& key, Fn&& callback) -> bool {
            auto snapshot = children();
            auto child    = [&] {
                if constexpr (std::integral<Key>)
                    return snapshot.at(key);
                else
                    return snapshot.find(key);
            }();
            if (!child) return false;
            NodeHandle<T> typed;
            typed.state    = child->state;
            typed.identity = child->identity;
            return typed.with(std::forward<Fn>(callback));
        }
        /**
         * @brief Visit matching direct children in captured insertion order without holding structure locks.
         * @tparam T Required dynamic type; other children are skipped.
         * @param callback Operation receiving each T&. Exceptions stop iteration and propagate.
         * @note Removed children are skipped and newly inserted children are not visited in this pass.
         * Parallel-domain boundaries still require post(); use edit_children() for efficient batch mutations.
         */
        template<class T = Node, class Fn>
        auto for_each_child(Fn&& callback) -> void {
            for (const auto& entry : children()) with_child<T>(entry.node.id(), callback);
        }
        /** @brief Enable/disable updates for this node and its descendants, without changing tree membership. */
        auto set_enabled(bool value) -> void { enabled = value; }
        /** @brief Return the local update gate; the scheduler also applies all ancestor gates. */
        virtual auto updates_enabled() const -> bool { return enabled; }
        /**
         * @brief Mark this subtree as an independent parallel execution domain.
         * @param value Whether this subtree is eligible when Processer enables parallel updates.
         * @throws std::logic_error If the tree is running.
         * @note Nested marks share the outermost domain. The processor root always remains on the coordinator.
         * Keep SDL/graphics operations in unmarked main-thread nodes; publish snapshots or post cross-domain work.
         */
        auto set_parallel(bool value) -> void;
        /** @brief Return the configured parallel mark, independent of the Processer setting. */
        auto is_parallel() const -> bool { return parallel; }
        /**
         * @brief Enter the running tree, parent before children, on each mount or run restart.
         * @note Called even when updates are disabled. An exception stops the run and triggers exit for entered nodes.
         */
        virtual auto enter_tree() -> void {}
        /**
         * @brief Initialize after children are ready, once per instance on its first running-tree attachment.
         * @note Not repeated after detach/reattach or a run restart, even if this callback previously threw.
         */
        virtual auto ready() -> void {}
        /**
         * @brief Perform one variable-step logic update, before enabled descendants.
         * @param context Measured/clamped delta, frame counters, and immutable input for this phase.
         * @note Runs in the owning execution domain. Structural edits become visible next frame.
         */
        virtual auto process(const FrameContext& context) -> void {}
        /**
         * @brief Perform one fixed-step update, before enabled descendants.
         * @param context Fixed delta and current physics tick; several ticks may occur in one logic frame.
         * @note The entire tree finishes each physics phase before the next phase starts.
         */
        virtual auto physics_process(const FrameContext& context) -> void {}
        /**
         * @brief Leave the running tree, children before parent, on detach, destruction, or run termination.
         * @note Called once per successful entry attempt. Cleanup continues if this callback throws; the first
         * failure is rethrown by run(). Destructors never invoke this virtual function. Submissions are closed at shutdown.
         */
        virtual auto exit_tree() -> void {}
    };

    namespace detail {
        auto node_children(const std::weak_ptr<NodeState>& state) -> ChildrenSnapshot;
        auto post_node(const std::weak_ptr<NodeState>& state,
                       std::function<void(Node&)> callback,
                       std::function<void(NodeError)> complete) -> bool;
    } // namespace detail
    template<class T>
    auto NodeHandle<T>::children() const -> ChildrenSnapshot {
        return detail::node_children(state);
    }
    template<class T>
    auto NodeHandle<T>::post(std::function<void(T&)> callback, std::function<void(NodeError)> complete) const -> bool {
        if (!callback) throw std::invalid_argument("empty node callback");
        return detail::post_node(
            state,
            [callback = std::move(callback)](Node& node) {
                auto* typed = dynamic_cast<T*>(&node);
                if (!typed) throw std::invalid_argument("wrong node type");
                callback(*typed);
            },
            std::move(complete));
    }

    /**
     * @brief Explicitly published immutable application state for cross-domain readers.
     * @tparam T Snapshot value type. Any indirectly referenced data must also be immutable or independently synchronized.
     * @note Readers retain old versions safely while publishers replace the current version atomically.
     */
    template<class T>
    class PublishedState {
        std::atomic<std::shared_ptr<const T>> value;

    public:
        /** @brief Publish an initial non-null snapshot; construction/allocation exceptions propagate. */
        explicit PublishedState(T initial) : value(std::make_shared<const T>(std::move(initial))) {}
        /** @brief Atomically publish a new immutable value; old readers retain their previous snapshot. */
        auto publish(T next) -> void { value.store(std::make_shared<const T>(std::move(next))); }
        /** @brief Acquire the current non-null snapshot from any thread. */
        auto snapshot() const -> std::shared_ptr<const T> { return value.load(); }
    };
} // namespace gkit::core
