#include "gkit/math/vector3.hpp"
#include "gkit/core/reflect/registry.hpp"

namespace gkit::math {
    static auto regist_holder = gkit::core::reflect::RegistHolder([]() {
        auto& db = gkit::core::reflect::ClassDB::instance();
        db.regist<Vector3>("Vector3")
            .add_field("Vector3", "x", &Vector3::x)
            .add_field("Vector3", "y", &Vector3::y)
            .add_field("Vector3", "z", &Vector3::z);
    });

    Vector3::Vector3(float v) noexcept : x(v), y(v), z(v) {}
    Vector3::Vector3(float x, float y, float z) noexcept : x(x), y(y), z(z) {}
    Vector3::Vector3(const Vector2& v, float z) noexcept : x(v.x), y(v.y), z(z) {}
    Vector3::Vector3(const Vector3&& other) noexcept : x(other.x), y(other.y), z(other.z) {}
} // namespace gkit::math
