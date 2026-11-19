#pragma once

#include "gkit/core/unique_object.hpp"

#include <atomic>
#include <functional>
#include <utility>
#include <vector>

namespace gkit {
    class Processer;
}

namespace gkit::core {
    namespace detail {
        struct NodeState;
    }

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

        auto find_ancestor_if(bool (*matches)(const Node&)) const -> std::optional<ObjectId>;

    public:
        /** @brief Register the node type and its name property in the supplied registry. */
        static auto _regist_method(reflect::ClassDB& db) -> void;

        using Completion = std::function<void(EditResult)>;

        Node();
        ~Node() override;
        Node(const Node&)                    = delete;
        auto operator=(const Node&) -> Node& = delete;

        [[nodiscard]] auto id() const noexcept -> ObjectId;
        [[nodiscard]] auto parent() const -> std::optional<ObjectId>;

        /**
         * @brief Find the nearest ancestor of type @p T, starting at the immediate parent.
         * @tparam T A node type. Derived instances also match.
         * @return The matching ID, or std::nullopt if the parent chain has no match.
         * @note The search observes one tree topology. Use ObjectId::with<T>() to borrow the result later.
         */
        template<std::derived_from<Node> T>
        [[nodiscard]] auto find_ancestor() const -> std::optional<ObjectId>;

        /** @brief Read the current node name under the tree lock. */
        [[nodiscard]] auto get_name() const -> std::string { return name(); }

        /** @brief Submit a rename through the tree edit queue. Use rename() to receive the result. */
        auto set_name(std::string name) -> void { rename(std::move(name)); }

        [[nodiscard]] auto name() const -> std::string;
        [[nodiscard]] auto find_child(const std::string& name) const -> std::optional<ObjectId>;
        [[nodiscard]] auto children() const -> std::vector<ChildInfo>;

        /** @brief Edits complete inline on stopped trees or at a frame boundary during run().
         * Adds from a derived constructor are committed after UniqueObject finishes construction; their callbacks
         * must outlive the constructor. Completion runs outside tree locks. Receive rejected additions and detached owners there. */
        auto add_child(std::string name, UniqueObject child, Completion complete = {}) -> void;
        auto detach_child(ObjectId child, Completion complete) -> void;
        auto remove_child(ObjectId child, Completion complete = {}) -> void;
        auto rename(std::string name, Completion complete = {}) -> void;
        auto reparent(ObjectId parent, Completion complete = {}) -> void;

        /** @brief Schedule custom code on the coordinator. False means the request was not accepted. */
        auto post(std::function<void(Node&)> callback, std::function<void(NodeError)> complete = {}) -> bool;

        auto set_enabled(bool value) noexcept -> void { enabled.store(value); }
        [[nodiscard]] virtual auto updates_enabled() const -> bool { return enabled.load(); }
        auto set_parallel(bool value) noexcept -> void { parallel.store(value); }
        [[nodiscard]] auto is_parallel() const noexcept -> bool { return parallel.load(); }

        /** @brief Called parent-first on entry and again after reattachment. */
        virtual auto enter_tree() -> void {}

        /** @brief Called once, after all children are ready. */
        virtual auto ready() -> void {}

        /**
         * @brief Run one logic update after input and fixed-step physics.
         * @param delta Elapsed frame time in seconds, capped by the Processer configuration.
         */
        virtual auto process(double delta) -> void {}

        /**
         * @brief Run one fixed-step physics update using the current frame's input.
         * @param delta Configured duration of one physics step in seconds.
         */
        virtual auto physics_process(double delta) -> void {}

        /** @brief Called child-first on removal or shutdown. */
        virtual auto exit_tree() -> void {}
    };

    template<std::derived_from<Node> T>
    auto Node::find_ancestor() const -> std::optional<ObjectId> {
        return find_ancestor_if([](const Node& ancestor) -> bool {
            return dynamic_cast<const T*>(&ancestor) != nullptr;
        });
    }

} // namespace gkit::core
