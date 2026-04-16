#include "gkit/math/vector2.hpp"

gkit::math::Vector2::Vector2(float32 v) noexcept : x(v), y(v) { }
gkit::math::Vector2::Vector2(float32 x, float32 y) noexcept : x(x), y(y) { }
gkit::math::Vector2::Vector2(const gkit::math::Vector2& other)  noexcept : x(other.x), y(other.y) { }
gkit::math::Vector2::Vector2(const gkit::math::Vector2&& other) noexcept : x(other.x), y(other.y) { }
