#include <gkit/math/vector3.hpp>
#include <string>
#include <format>
#include <iostream>

using gkit::math::Vector3;

auto vec_str(Vector3& vec) -> std::string {
    auto [x, y, z] = vec.properties();
    return std::format("x = {}, y = {}, z = {}", x, y, z);
}

auto main() -> int {
    // constructure test
    Vector3 vec1({1.0f, 2.0f, 3.0f});
    std::cout << "vec1: " << vec_str(vec1) << std::endl;

    Vector3 vec2(vec1);
    std::cout << "vec2(construct from vec1): " << vec_str(vec2) << std::endl;

    auto vec3 = vec1;
    std::cout << "vec3(construct with = from vec1): " << vec_str(vec3) << std::endl;

    // Arithmetic operators test
    auto vec4 = vec2 + vec3;
    std::cout << "vec4(vec2 + vec3): " << vec_str(vec4) << std::endl;

    auto vec5 = vec2 - vec3;
    std::cout << "vec5(vec2 - vec3): " << vec_str(vec5) << std::endl;

    auto vec6 = vec2 * 10;
    std::cout << "vec6(vec2 * 10): " << vec_str(vec6) << std::endl;

    auto vec7 = vec2 / 5;
    std::cout << "vec7(vec2 / 5): " << vec_str(vec7) << std::endl;

    // other methods
    std::cout << "vec1.length(): " << vec1.length() << std::endl;
    vec1.normalization();
    std::cout << "vec1.normalization(): " << vec_str(vec1) << std::endl;
    std::cout << "vec1.length() after normalization: " << vec1.length() << std::endl;
}