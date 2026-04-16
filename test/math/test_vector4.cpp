#include <gkit/math/vector4.hpp>
#include <string>
#include <format>
#include <iostream>

using gkit::math::Vector4;

auto vec_str(const gkit::math::Vector4& vec) -> std::string {
    auto [x, y, z, w] = vec.properties();
    return std::format("x = {}, y = {}, z = {}, w = {}", x, y, z, w);
}

auto main() -> int {
    // constructor test
    Vector4 vec1{1.0f, 2.0f, 3.0f, 4.0f};
    std::cout << "vec1: " << vec_str(vec1) << std::endl;

    Vector4 vec2(vec1);
    std::cout << "vec2(construct from vec1): " << vec_str(vec2) << std::endl;

    auto vec3 = vec1;
    std::cout << "vec3(construct with = from vec1): " << vec_str(vec3) << std::endl;

    // Arithmetic operators test
    auto vec4 = vec2 + vec3;
    std::cout << "vec4(vec2 + vec3): " << vec_str(vec4) << std::endl;

    auto vec5 = vec2 - vec3;
    std::cout << "vec5(vec2 - vec3): " << vec_str(vec5) << std::endl;

    auto vec6 = vec2 * 10.0f;
    std::cout << "vec6(vec2 * 10): " << vec_str(vec6) << std::endl;

    auto vec7 = vec2 / 5.0f;
    std::cout << "vec7(vec2 / 5): " << vec_str(vec7) << std::endl;

    // other methods
    std::cout << "vec1.length(): " << vec1.length() << std::endl;
    vec1 = Vector4::normalize(vec1);
    std::cout << "vec1 after normalize: " << vec_str(vec1) << std::endl;
    std::cout << "vec1.length() after normalize: " << vec1.length() << std::endl;
}