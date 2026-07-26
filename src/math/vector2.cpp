#include "gkit/math/vector2.hpp"
#include "gkit/core/reflect/registry.hpp"

namespace gkit::math {
    auto regist_holder = core::reflect::RegistHolder([]() {
        auto& db = gkit::core::reflect::ClassDB::instance();
        db.regist<Vector2>("Vector2")
            .add_field("Vector2", "x", &Vector2::x)
            .add_field("Vector2", "y", &Vector2::y);
    });

    Vector2::Vector2(float v) noexcept : x(v), y(v) {}
    Vector2::Vector2(float x, float y) noexcept : x(x), y(y) {}
    Vector2::Vector2(const gkit::math::Vector2&& other) noexcept : x(other.x), y(other.y) {}
} // namespace gkit::math
