#include "test_utils.hpp"

#include <format>
#include <string>

#include <gkit/math/vector4.hpp>

using gkit::math::Vector4;

auto vec_str(const gkit::math::Vector4& vec) -> std::string {
    auto [x, y, z, w] = vec.properties();
    return std::format("x = {}, y = {}, z = {}, w = {}", x, y, z, w);
}


auto test_constructors() -> bool {
    gkit::test::logln("=== constructors ===");

    Vector4 vec1{1.0f, 2.0f, 3.0f, 4.0f};
    gkit::test::logln("  vec1: {}", vec_str(vec1));

    Vector4 vec2(vec1);
    gkit::test::logln("  vec2(construct from vec1): {}", vec_str(vec2));

    auto vec3 = vec1;
    gkit::test::logln("  vec3(construct with = from vec1): {}", vec_str(vec3));

    gkit::test::assert_if(vec2.x == 1.0f && vec2.y == 2.0f && vec2.z == 3.0f && vec2.w == 4.0f,
                          "copy constructor failed");
    gkit::test::assert_if(vec3.x == 1.0f && vec3.y == 2.0f && vec3.z == 3.0f && vec3.w == 4.0f,
                          "copy assignment failed");
    return true;
}


auto test_arithmetic() -> bool {
    gkit::test::logln("=== arithmetic operators ===");

    Vector4 vec1{1.0f, 2.0f, 3.0f, 4.0f};
    auto vec2 = vec1;
    auto vec3 = vec1;

    auto vec4 = vec2 + vec3;
    gkit::test::assert_if(vec4.x == 2.0f && vec4.y == 4.0f && vec4.z == 6.0f && vec4.w == 8.0f, "vec2 + vec3 failed");
    gkit::test::logln("  vec4(vec2 + vec3): {}", vec_str(vec4));

    auto vec5 = vec2 - vec3;
    gkit::test::assert_if(vec5.x == 0.0f && vec5.y == 0.0f && vec5.z == 0.0f && vec5.w == 0.0f, "vec2 - vec3 failed");
    gkit::test::logln("  vec5(vec2 - vec3): {}", vec_str(vec5));

    auto vec6 = vec2 * 10.0f;
    gkit::test::assert_if(vec6.x == 10.0f && vec6.y == 20.0f && vec6.z == 30.0f && vec6.w == 40.0f, "vec2 * 10 failed");
    gkit::test::logln("  vec6(vec2 * 10): {}", vec_str(vec6));

    auto vec7 = vec2 / 5.0f;
    gkit::test::assert_if(vec7.x == 0.2f && vec7.y == 0.4f && vec7.z == 0.6f && vec7.w == 0.8f, "vec2 / 5 failed");
    gkit::test::logln("  vec7(vec2 / 5): {}", vec_str(vec7));
    return true;
}


auto test_length_and_normalize() -> bool {
    gkit::test::logln("=== length and normalize ===");

    Vector4 vec1{1.0f, 2.0f, 3.0f, 4.0f};
    gkit::test::logln("  vec1.length(): {}", vec1.length());
    gkit::test::assert_if(vec1.length() > 0.0f, "vec1.length() must be positive");

    vec1 = vec1.normalize();
    gkit::test::logln("  vec1 after normalize: {}", vec_str(vec1));
    gkit::test::logln("  vec1.length() after normalize: {}", vec1.length());
    gkit::test::assert_if(vec1.length() > 0.999f && vec1.length() < 1.001f, "normalize failed");
    return true;
}


auto main() -> int {
    auto test_runner = gkit::test::TestRunner()
                           .add_test_func(test_constructors)
                           .add_test_func(test_arithmetic)
                           .add_test_func(test_length_and_normalize);

    test_runner.run();
    return 0;
}
