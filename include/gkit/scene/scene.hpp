/** @file scene.hpp
 * @brief Composable activation and pause boundaries built on core::Node.
 */
#pragma once

#include "gkit/core/node.hpp"

namespace gkit::scene {
    /**
     * @brief A composable scene boundary that can contain Node and Scene descendants.
     * @note Inactive or paused scenes suppress both logic and physics updates throughout their subtree.
     * These states do not change ownership or trigger enter/ready/exit callbacks. An active child scene cannot override
     * a disabled, inactive or paused ancestor. Scene uses the same ownership and execution-domain rules as core::Node.
     */
    class Scene : public core::Node {
        std::atomic<bool> active = true;
        std::atomic<bool> paused = false;

    public:
        /** @brief Register the scene type and its active and paused properties in the supplied registry. */
        static auto _regist_method(core::reflect::ClassDB& db) -> void;

        /** @brief Construct an active, unpaused scene. */
        Scene();

        /** @brief Return the local activation flag, independent of ancestor state. */
        [[nodiscard]] auto get_active() const -> bool { return active.load(); }

        /** @brief Set whether this scene permits subtree updates, subject to pause and ancestor gates. */
        auto set_active(bool value) -> void { active.store(value); }

        /** @brief Return the local pause flag, independent of ancestor state. */
        [[nodiscard]] auto get_paused() const -> bool { return paused.load(); }

        /** @brief Suspend or resume subtree updates while preserving the activation state. */
        auto set_paused(bool value) -> void { paused.store(value); }

        /** @brief Allow updates subject to pause, Node enablement, and all ancestor gates. */
        auto activate() -> void { set_active(true); }

        /** @brief Suppress subtree updates while preserving membership and application state. */
        auto deactivate() -> void { set_active(false); }

        /** @brief Pause subtree updates without changing the activation state. */
        auto pause() -> void { set_paused(true); }

        /** @brief Clear the local pause flag; ancestors may still prevent updates. */
        auto resume() -> void { set_paused(false); }

        /** @brief Return the local activation flag, independent of ancestor state. */
        [[nodiscard]] auto is_active() const -> bool { return get_active(); }

        /** @brief Return the local pause flag, independent of ancestor state. */
        [[nodiscard]] auto is_paused() const -> bool { return get_paused(); }

        /** @brief Combine Node enablement, activation and pause into the local update gate. */
        [[nodiscard]] auto updates_enabled() const -> bool override {
            return Node::updates_enabled() && active && !paused;
        }
    };
} // namespace gkit::scene
