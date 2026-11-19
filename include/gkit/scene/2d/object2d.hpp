/** @file object2d.hpp
 * @brief A tree node with explicit two-dimensional transform state.
 */
#pragma once

#include "gkit/core/node.hpp"
#include "gkit/scene/2d/transform2d.hpp"

#include <mutex>

namespace gkit::scene {
    /**
     * @brief A Node carrying a Transform2D, usable beneath Node or Scene parents.
     * @note The transform is local application state; this class does not compute inherited/world transforms.
     * Transform reads and writes are synchronized.
     */
    class Object2D : public gkit::core::Node {
    public:
        /** @brief Register the transform-bearing node type in the supplied registry. */
        static auto _regist_method(core::reflect::ClassDB& db) -> void;

        /** @brief Construct an Object2D with an identity transform. */
        Object2D();

        /** @brief Release transform state; Node lifecycle callbacks are managed separately. */
        ~Object2D() override = default;

    public:
        // Transform2D
        /** @brief Read the local transform as one synchronized value. */
        auto get_transform() const -> Transform2D {
            std::lock_guard guard(mutex);
            return transform;
        }

        /** @brief Replace the local transform as one synchronized value. */
        auto set_transform(const Transform2D& value) -> void {
            std::lock_guard guard(mutex);
            transform = value;
        }

        /** @brief Copy the local two-dimensional position. */
        inline auto get_position() const -> math::Vector2 {
            std::lock_guard guard(mutex);
            return transform.get_position();
        }

        /** @brief Replace the local position; no parent transform is applied. */
        inline auto set_position(const math::Vector2& position) -> void {
            std::lock_guard guard(mutex);
            transform.set_position(position);
        }

        /** @brief Return the stored local rotation value, using the existing Transform2D convention. */
        inline auto get_rotation() const -> float {
            std::lock_guard guard(mutex);
            return transform.get_rotation();
        }

        /** @brief Replace the stored local rotation value without conversion or normalization. */
        inline auto set_rotation(const float rotation) -> void {
            std::lock_guard guard(mutex);
            transform.set_rotation(rotation);
        }

        /** @brief Copy the local scale factors. */
        inline auto get_scale() const -> math::Vector2 {
            std::lock_guard guard(mutex);
            return transform.get_scale();
        }

        /** @brief Replace the local scale factors without changing descendants. */
        inline auto set_scale(const math::Vector2& scale) -> void {
            std::lock_guard guard(mutex);
            transform.set_scale(scale);
        }

    private:
        mutable std::mutex mutex;
        Transform2D transform;
    };
} // namespace gkit::scene
