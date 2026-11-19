#include "gkit/math/vector3.hpp"

namespace gkit::math {

    Vector3::Vector3(float v) noexcept : x(v), y(v), z(v) {}


    Vector3::Vector3(float x, float y, float z) noexcept : x(x), y(y), z(z) {}


    Vector3::Vector3(const Vector2& v, float z) noexcept : x(v.x), y(v.y), z(z) {}
} // namespace gkit::math
