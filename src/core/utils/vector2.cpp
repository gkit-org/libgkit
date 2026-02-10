#include <core/include/utils/vector2.hpp>


gkit::utils::Vector2::Vector2(float x, float y) noexcept : x(x), y(y) { }
gkit::utils::Vector2::Vector2(const gkit::utils::Vector2& other)  noexcept : x(other.x), y(other.y) { }
gkit::utils::Vector2::Vector2(const gkit::utils::Vector2&& other) noexcept : x(other.x), y(other.y) { }


auto gkit::utils::Vector2::normalization() -> void {
    auto len = this->length();
    this->x /= len; this->y /= len;
}


auto gkit::utils::Vector2::zero() -> const Vector2& {
    static Vector2 zero = Vector2(0.0f, 0.0f);
    return zero;
}
