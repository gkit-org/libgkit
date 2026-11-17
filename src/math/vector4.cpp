#include "gkit/math/vector4.hpp"

namespace gkit::math {

    Vector4::Vector4(float v) noexcept : x(v), y(v), z(v), w(v) {}
    Vector4::Vector4(float x, float y, float z, float w) noexcept : x(x), y(y), z(z), w(w) {}
    Vector4::Vector4(const Vector3& v, float w) noexcept : x(v.x), y(v.y), z(v.z), w(w) {}
    Vector4::Vector4(const Vector2& v, float z, float w) noexcept : x(v.x), y(v.y), z(z), w(w) {}
} // namespace gkit::math
