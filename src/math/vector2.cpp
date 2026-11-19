#include "gkit/math/vector2.hpp"

namespace gkit::math {

    Vector2::Vector2(float v) noexcept : x(v), y(v) {}


    Vector2::Vector2(float x, float y) noexcept : x(x), y(y) {}
} // namespace gkit::math
