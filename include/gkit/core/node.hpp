#pragma once

#include "gkit/core/input/input.hpp"
#include "gkit/core/unique_object.hpp"

#include <atomic>
#include <functional>
#include <vector>

namespace gkit {
    class Processer;
}
namespace gkit::core {
    namespace detail {
        struct NodeState;
    }
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
        InvalidEdit, ///< The requested tree operation failed validation.
        CallbackFailed, ///< The posted callback threw; run() also propagates the exception after cleanup.
        Stopped ///< The scheduler cancelled this operation during shutdown.
    };

    struct ChildInfo {
        std::string name;
        ObjectId id;
    };

    struct EditResult {
        NodeError error = NodeError::None;
        /// Detached subtree, or the original addition on rejection or cancellation.
        std::optional<UniqueObject> owner;
    };

    /** Create tree nodes through UniqueObject::create<T>(). Tree operations are synchronized. Derived classes synchronize their own fields. */
    class Node : public Object {
        friend struct detail::NodeState;
        friend class gkit::Processer;
        std::unique_ptr<detail::NodeState> state;
        std::atomic<bool> enabled{true}, parallel{false};

    public:
        using Completion = std::function<void(EditResult)>;
        Node();
        ~Node() override;
        Node(const Node&)                    = delete;
        auto operator=(const Node&) -> Node& = delete;
        auto id() const noexcept -> ObjectId;
        auto parent() const -> std::optional<ObjectId>;
        auto name() const -> std::string;
        auto find_child(const std::string& name) const -> std::optional<ObjectId>;
        auto children() const -> std::vector<ChildInfo>;

        /** Edits complete inline on stopped trees or at a frame boundary during run().
         * Completion runs outside tree locks. Receive rejected additions and detached owners there. */
        auto add_child(std::string name, UniqueObject child, Completion complete = {}) -> void;
        auto detach_child(ObjectId child, Completion complete) -> void;
        auto remove_child(ObjectId child, Completion complete = {}) -> void;
        auto rename(std::string name, Completion complete = {}) -> void;
        auto reparent(ObjectId parent, Completion complete = {}) -> void;
        /** Schedule custom code on the coordinator. False means the request was not accepted. */
        auto post(std::function<void(Node&)> callback, std::function<void(NodeError)> complete = {}) -> bool;

        auto set_enabled(bool value) noexcept -> void { enabled.store(value); }
        virtual auto updates_enabled() const -> bool { return enabled.load(); }
        auto set_parallel(bool value) noexcept -> void { parallel.store(value); }
        auto is_parallel() const noexcept -> bool { return parallel.load(); }
        /** Called parent-first on entry and again after reattachment. */
        virtual auto enter_tree() -> void {}
        /** Called once, after all children are ready. */
        virtual auto ready() -> void {}
        virtual auto process(const FrameContext&) -> void {}
        virtual auto physics_process(const FrameContext&) -> void {}
        /** Called child-first on removal or shutdown. */
        virtual auto exit_tree() -> void {}
    };
} // namespace gkit::core
