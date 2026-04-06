#include <gkit/math/vector2.hpp>


gkit::math::Vector2::Vector2(float x, float y) noexcept : x(x), y(y) { }
gkit::math::Vector2::Vector2(const gkit::math::Vector2& other)  noexcept : x(other.x), y(other.y) { }
gkit::math::Vector2::Vector2(const gkit::math::Vector2&& other) noexcept : x(other.x), y(other.y) { }


auto gkit::math::Vector2::normalization() -> void {
    auto len = this->length();
    this->x /= len; this->y /= len;
}


auto gkit::math::Vector2::zero() -> const Vector2& {
    static Vector2 zero = Vector2(0.0f, 0.0f);
    return zero;
}
