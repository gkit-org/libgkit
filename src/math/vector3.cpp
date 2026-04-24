#include "gkit/math/vector3.hpp"

gkit::math::Vector3::Vector3(float v) noexcept : x(v), y(v), z(v) { }
gkit::math::Vector3::Vector3(float x, float y, float z) noexcept : x(x), y(y), z(z) { }
gkit::math::Vector3::Vector3(const gkit::math::Vector2& v, float z) noexcept : x(v.x), y(v.y), z(z) { }
gkit::math::Vector3::Vector3(const gkit::math::Vector3&& other) noexcept : x(other.x), y(other.y), z(other.z) { }