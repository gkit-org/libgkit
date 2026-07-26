#include "gkit/math/vector4.hpp"
#include "gkit/core/reflect/registry.hpp"

namespace gkit::math {
    static auto regist_holder = core::reflect::RegistHolder([]() {
        auto& db = core::reflect::ClassDB::instance();
        db.regist<Vector4>("Vector4")
            .add_field("Vector4", "x", &Vector4::x)
            .add_field("Vector4", "y", &Vector4::y)
            .add_field("Vector4", "z", &Vector4::z)
            .add_field("Vector4", "w", &Vector4::w);
    });

    Vector4::Vector4(float v) noexcept : x(v), y(v), z(v), w(v) {}
    Vector4::Vector4(float x, float y, float z, float w) noexcept : x(x), y(y), z(z), w(w) {}
    Vector4::Vector4(const Vector3& v, float w) noexcept : x(v.x), y(v.y), z(v.z), w(w) {}
    Vector4::Vector4(const Vector2& v, float z, float w) noexcept : x(v.x), y(v.y), z(z), w(w) {}
    Vector4::Vector4(const Vector4&& other) noexcept :
        x(other.x), y(other.y), z(other.z), w(other.w) {}
} // namespace gkit::math
