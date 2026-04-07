#pragma once

#include "gkit/math/vector2.hpp"

namespace gkit::scene {
    class Transform2D {
    public:
        Transform2D() = default;
        ~Transform2D() = default;

    public:
        /*********************************
         * Getters
         *********************************/
        inline auto get_position() const -> math::Vector2 { return position; }
        inline auto get_rotation() const -> float { return rotation; }
        inline auto get_scale() const -> math::Vector2 { return scale; }

        /*********************************
         * Setters
         *********************************/
        inline auto set_position(const math::Vector2& position) -> void { this->position = position; }
        inline auto set_rotation(const float rotation) -> void { this->rotation = rotation; }
        inline auto set_scale(const math::Vector2& scale) -> void { this->scale = scale; }

    private:
        math::Vector2 position = {0.0f, 0.0f};
        float rotation = 0.0f;
        math::Vector2 scale {1.0f, 1.0f};
    };
}