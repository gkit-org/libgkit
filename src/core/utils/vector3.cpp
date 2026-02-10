#include "utils/vector3.hpp"


gkit::utils::Vector3::Vector3(float x, float y, float z) noexcept : x(x), y(y), z(z) {}
gkit::utils::Vector3::Vector3(const Vector3&  other) noexcept : x(other.x), y(other.y), z(other.z) {}
gkit::utils::Vector3::Vector3(const Vector3&& other) noexcept : x(other.x), y(other.y), z(other.z) {}


auto gkit::utils::Vector3::normalization() -> void {
    auto len = this->length();
    this->x /= len; this->y /= len; this->z /= len;
}


auto gkit::utils::Vector3::zero() -> const Vector3& {
    static Vector3 zero(0.0f, 0.0f, 0.0f);
    return zero;
}
