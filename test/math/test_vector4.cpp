#include <format>
#include <iostream>
#include <string>

#include <gkit/math/vector4.hpp>

using gkit::math::Vector4;

auto vec_str(const gkit::math::Vector4& vec) -> std::string {
    auto [x, y, z, w] = vec.properties();
    return std::format("x = {}, y = {}, z = {}, w = {}", x, y, z, w);
}

auto main() -> int {
    // constructor test
    Vector4 vec1{1.0f, 2.0f, 3.0f, 4.0f};
    std::cout << "vec1: " << vec_str(vec1) << '\n';

    Vector4 vec2(vec1);
    std::cout << "vec2(construct from vec1): " << vec_str(vec2) << '\n';

    auto vec3 = vec1;
    std::cout << "vec3(construct with = from vec1): " << vec_str(vec3) << '\n';

    // Arithmetic operators test
    auto vec4 = vec2 + vec3;
    std::cout << "vec4(vec2 + vec3): " << vec_str(vec4) << '\n';

    auto vec5 = vec2 - vec3;
    std::cout << "vec5(vec2 - vec3): " << vec_str(vec5) << '\n';

    auto vec6 = vec2 * 10.0f;
    std::cout << "vec6(vec2 * 10): " << vec_str(vec6) << '\n';

    auto vec7 = vec2 / 5.0f;
    std::cout << "vec7(vec2 / 5): " << vec_str(vec7) << '\n';

    // other methods
    std::cout << "vec1.length(): " << vec1.length() << '\n';
    vec1 = vec1.normalize();
    std::cout << "vec1 after normalize: " << vec_str(vec1) << '\n';
    std::cout << "vec1.length() after normalize: " << vec1.length() << '\n';
}