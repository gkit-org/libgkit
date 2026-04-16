#include "gkit/math/vector2.hpp"

gkit::math::Vector2::Vector2(float v) noexcept : x(v), y(v) { }
gkit::math::Vector2::Vector2(float x, float y) noexcept : x(x), y(y) { }
gkit::math::Vector2::Vector2(const gkit::math::Vector2& other)  noexcept : x(other.x), y(other.y) { }
gkit::math::Vector2::Vector2(const gkit::math::Vector2&& other) noexcept : x(other.x), y(other.y) { }
