#include "gkit/math/scalar.hpp"
#include "gkit/math/vector2.hpp"
#include "test_utils.hpp"

#include <cmath>
#include <string>

using gkit::math::Vector2;

auto vec_str(const Vector2& vec) -> std::string {
    return std::format("x = {:.4f}, y = {:.4f}", vec.x, vec.y);
}


auto test_constructors() -> bool {
    gkit::test::logln("=== constructors ===");

    Vector2 vec1(1.0f, 2.0f);
    gkit::test::logln("  vec1: {}", vec_str(vec1));

    Vector2 vec_fill(5.0f);
    gkit::test::assert_if(vec_fill.x == 5.0f && vec_fill.y == 5.0f, "fill constructor failed");
    gkit::test::logln("  vec_fill (Vector2(5.0f)): {}", vec_str(vec_fill));

    Vector2 vec2(3.0f, 4.0f);
    gkit::test::logln("  vec2: {}", vec_str(vec2));
    return true;
}


auto test_arithmetic() -> bool {
    gkit::test::logln("=== arithmetic operators ===");

    Vector2 vec1(1.0f, 2.0f);
    Vector2 vec2(3.0f, 4.0f);

    Vector2 vec_add = vec1 + vec2;
    gkit::test::assert_if(vec_add.x == 4.0f && vec_add.y == 6.0f, "vec1 + vec2 failed");
    gkit::test::logln("  vec1 + vec2: {}", vec_str(vec_add));

    Vector2 vec_sub = vec1 - vec2;
    gkit::test::assert_if(vec_sub.x == -2.0f && vec_sub.y == -2.0f, "vec1 - vec2 failed");
    gkit::test::logln("  vec1 - vec2: {}", vec_str(vec_sub));

    Vector2 vec_mul = vec1 * 10.0f;
    gkit::test::assert_if(vec_mul.x == 10.0f && vec_mul.y == 20.0f, "vec1 * 10 failed");
    gkit::test::logln("  vec1 * 10: {}", vec_str(vec_mul));

    Vector2 vec_div = vec1 / 10.0f;
    gkit::test::assert_if(std::abs(vec_div.x - 0.1f) < gkit::math::EPSILON32 &&
                              std::abs(vec_div.y - 0.2f) < gkit::math::EPSILON32,
                          "vec1 / 10 failed");
    gkit::test::logln("  vec1 / 10: {}", vec_str(vec_div));
    return true;
}


auto test_length_and_normalize() -> bool {
    gkit::test::logln("=== length and normalize ===");

    Vector2 vec1(1.0f, 2.0f);
    gkit::test::logln("  vec1.length(): {}", vec1.length());
    gkit::test::assert_if(std::abs(vec1.length() - std::sqrt(5.0f)) < gkit::math::EPSILON32, "vec1.length() failed");
    gkit::test::logln("  vec1.length_sq(): {}", vec1.length_sq());
    gkit::test::assert_if(std::abs(vec1.length_sq() - 5.0f) < gkit::math::EPSILON32, "vec1.length_sq() failed");

    vec1 = vec1.normalize();
    gkit::test::logln("  vec1 after normalize: {}", vec_str(vec1));
    gkit::test::assert_if(std::abs(vec1.length() - 1.0f) < gkit::math::EPSILON32, "normalize failed");

    Vector2 c(3.0f, 4.0f);
    Vector2 c_normalized = c.normalize();
    gkit::test::logln("  Vector2.normalize(3,4): {}", vec_str(c_normalized));
    gkit::test::assert_if(std::abs(c_normalized.length() - 1.0f) < gkit::math::EPSILON32, "normalize(3,4) failed");
    return true;
}


auto test_dot_and_cross() -> bool {
    gkit::test::logln("=== dot and cross product ===");

    Vector2 a(1.0f, 0.0f);
    Vector2 b(0.0f, 1.0f);

    float dot_result = Vector2::dot(a, b);
    gkit::test::assert_if(dot_result == 0.0f, "Vector2::dot(a, b) failed");
    gkit::test::logln("  Vector2::dot(a, b): {}", dot_result);

    float dot_a = Vector2::dot(a, a);
    gkit::test::assert_if(dot_a == 1.0f, "Vector2::dot(a, a) failed");

    float cross_result = Vector2::cross(a, b);
    gkit::test::assert_if(cross_result == 1.0f, "Vector2::cross(a, b) failed");
    gkit::test::logln("  Vector2::cross(a, b): {}", cross_result);
    return true;
}


auto test_lerp_min_max() -> bool {
    gkit::test::logln("=== lerp, min, max ===");

    Vector2 a(1.0f, 0.0f);
    Vector2 b(0.0f, 1.0f);

    Vector2 lerp_result = Vector2::lerp(a, b, 0.5f);
    gkit::test::assert_if(std::abs(lerp_result.x - 0.5f) < gkit::math::EPSILON32, "lerp.x failed");
    gkit::test::assert_if(std::abs(lerp_result.y - 0.5f) < gkit::math::EPSILON32, "lerp.y failed");
    gkit::test::logln("  Vector2::lerp((1,0), (0,1), 0.5): {}", vec_str(lerp_result));

    Vector2 min_result = Vector2::min(a, b);
    gkit::test::assert_if(min_result.x == 0.0f && min_result.y == 0.0f, "Vector2::min failed");
    gkit::test::logln("  Vector2::min((1,0), (0,1)): {}", vec_str(min_result));

    Vector2 max_result = Vector2::max(a, b);
    gkit::test::assert_if(max_result.x == 1.0f && max_result.y == 1.0f, "Vector2::max failed");
    gkit::test::logln("  Vector2::max((1,0), (0,1)): {}", vec_str(max_result));
    return true;
}


auto test_perp_reflect_distance() -> bool {
    gkit::test::logln("=== perp, reflect, distance ===");

    Vector2 a(1.0f, 0.0f);

    Vector2 perp_result = Vector2::perp(a);
    gkit::test::assert_if(perp_result.x == 0.0f && perp_result.y == 1.0f, "Vector2::perp failed");
    gkit::test::logln("  Vector2::perp((1,0)): {}", vec_str(perp_result));

    Vector2 v(1.0f, 1.0f);
    Vector2 n(0.0f, 1.0f);
    Vector2 reflect_result = Vector2::reflect(v, n);
    gkit::test::assert_if(std::abs(reflect_result.x - 1.0f) < gkit::math::EPSILON32, "reflect.x failed");
    gkit::test::assert_if(std::abs(reflect_result.y - (-1.0f)) < gkit::math::EPSILON32, "reflect.y failed");
    gkit::test::logln("  Vector2::reflect((1,1), (0,1)): {}", vec_str(reflect_result));

    Vector2 p1(0.0f, 0.0f);
    Vector2 p2(3.0f, 4.0f);
    float dist = Vector2::distance(p1, p2);
    gkit::test::assert_if(std::abs(dist - 5.0f) < gkit::math::EPSILON32, "Vector2::distance failed");
    gkit::test::logln("  Vector2::distance((0,0), (3,4)): {}", dist);
    return true;
}


auto main() -> int {
    auto test_runner = gkit::test::TestRunner()
                           .add_test_func(test_constructors)
                           .add_test_func(test_arithmetic)
                           .add_test_func(test_length_and_normalize)
                           .add_test_func(test_dot_and_cross)
                           .add_test_func(test_lerp_min_max)
                           .add_test_func(test_perp_reflect_distance);

    test_runner.run();
    return 0;
}
