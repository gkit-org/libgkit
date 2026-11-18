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
        /** @brief Frame duration in seconds, clamped for process() and fixed for physics_process(). */
        double delta = 0;
        /** @brief Zero-based logic-frame number, reset on each run(). */
        std::uint64_t frame = 0;
        /** @brief Zero-based physics tick, or completed tick count during process(). */
        std::uint64_t physics_tick = 0;
        /** @brief Immutable input snapshot, always present during scheduled callbacks and safe to retain. */
        std::shared_ptr<const input::Snapshot> input;
    };

    /** @brief Completion status for accepted edits and posted callbacks. */
    enum class NodeError {
        /** @brief The operation completed successfully. */
        None,
        /** @brief The anchor/target expired or left its submitting runtime before execution. */
        Expired,
        /** @brief The requested tree operation failed validation. */
        InvalidEdit,
        /** @brief The posted callback threw an exception that run() propagates after cleanup. */
        CallbackFailed,
        /** @brief The scheduler cancelled this operation during shutdown. */
        Stopped
    };

    struct ChildInfo {
        std::string name;
        ObjectId id;
    };

    struct EditResult {
        NodeError error = NodeError::None;
        /** @brief Detached subtree, or the original addition on rejection or cancellation. */
        std::optional<UniqueObject> owner;
    };

    /**
     * @brief Create tree nodes through UniqueObject::create<T>().
     * Tree operations are synchronized. Derived classes synchronize their own fields.
     */
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

        /** @brief Edits complete inline on stopped trees or at a frame boundary during run().
         * Completion runs outside tree locks. Receive rejected additions and detached owners there. */
        auto add_child(std::string name, UniqueObject child, Completion complete = {}) -> void;
        auto detach_child(ObjectId child, Completion complete) -> void;
        auto remove_child(ObjectId child, Completion complete = {}) -> void;
        auto rename(std::string name, Completion complete = {}) -> void;
        auto reparent(ObjectId parent, Completion complete = {}) -> void;
        /** @brief Schedule custom code on the coordinator. False means the request was not accepted. */
        auto post(std::function<void(Node&)> callback, std::function<void(NodeError)> complete = {}) -> bool;

        auto set_enabled(bool value) noexcept -> void { enabled.store(value); }
        virtual auto updates_enabled() const -> bool { return enabled.load(); }
        auto set_parallel(bool value) noexcept -> void { parallel.store(value); }
        auto is_parallel() const noexcept -> bool { return parallel.load(); }
        /** @brief Called parent-first on entry and again after reattachment. */
        virtual auto enter_tree() -> void {}
        /** @brief Called once, after all children are ready. */
        virtual auto ready() -> void {}
        virtual auto process(const FrameContext&) -> void {}
        virtual auto physics_process(const FrameContext&) -> void {}
        /** @brief Called child-first on removal or shutdown. */
        virtual auto exit_tree() -> void {}
    };
} // namespace gkit::core
