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
        bool active = true;
        bool paused = false;

    public:
        /** @brief Construct an active, unpaused scene and register its reflected properties. */
        Scene();
        /** @brief Allow updates subject to pause, Node enablement, and all ancestor gates. */
        auto activate() -> void { active = true; }
        /** @brief Suppress subtree updates while preserving membership and application state. */
        auto deactivate() -> void { active = false; }
        /** @brief Pause subtree updates without changing the activation state. */
        auto pause() -> void { paused = true; }
        /** @brief Clear the local pause flag; ancestors may still prevent updates. */
        auto resume() -> void { paused = false; }
        /** @brief Return the local activation flag, independent of ancestor state. */
        auto is_active() const -> bool { return active; }
        /** @brief Return the local pause flag, independent of ancestor state. */
        auto is_paused() const -> bool { return paused; }
        /** @brief Combine Node enablement, activation and pause into the local update gate. */
        auto updates_enabled() const -> bool override { return Node::updates_enabled() && active && !paused; }
    };
} // namespace gkit::scene
