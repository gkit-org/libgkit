/** @file object2d.hpp
 * @brief A tree node with explicit two-dimensional transform state.
 */
#pragma once

#include "gkit/core/node.hpp"
#include "gkit/scene/2d/transform2d.hpp"

namespace gkit::scene {
    /**
     * @brief A Node carrying a Transform2D, usable beneath Node or Scene parents.
     * @note The transform is local application state; this class does not compute inherited/world transforms.
     * Access follows core::Node execution-domain rules. Publish copied values for cross-domain readers.
     */
    class Object2D : public gkit::core::Node {
    public:
        /** @brief Construct an identity transform and register the Object2D reflection type. */
        Object2D();
        /** @brief Release transform state; Node lifecycle callbacks are managed separately. */
        virtual ~Object2D() = default;

    public:
        /***********************************
         * Transform2D
         **********************************/
        /** @brief Return mutable local transform state; do not retain this reference beyond the owning-domain callback. */
        inline auto get_transform() -> Transform2D& { return transform; }
        /** @brief Return local transform state by const reference with the same callback-lifetime restriction. */
        inline auto get_transform() const -> const Transform2D& { return transform; }
        /** @brief Copy the local two-dimensional position. */
        inline auto get_position() const -> math::Vector2 { return transform.get_position(); }
        /** @brief Replace the local position; no parent transform is applied. */
        inline auto set_position(const math::Vector2& position) -> void { transform.set_position(position); }
        /** @brief Return the stored local rotation value, using the existing Transform2D convention. */
        inline auto get_rotation() const -> float { return transform.get_rotation(); }
        /** @brief Replace the stored local rotation value without conversion or normalization. */
        inline auto set_rotation(const float rotation) -> void { transform.set_rotation(rotation); }
        /** @brief Copy the local scale factors. */
        inline auto get_scale() const -> math::Vector2 { return transform.get_scale(); }
        /** @brief Replace the local scale factors without changing descendants. */
        inline auto set_scale(const math::Vector2& scale) -> void { transform.set_scale(scale); }

    private:
        Transform2D transform;
    };
} // namespace gkit::scene