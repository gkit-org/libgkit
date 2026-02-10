#include "utils/vector2.hpp"
#include <iostream>
#include <format>
#include <string>

using gkit::utils::Vector2;

auto vec_str(Vector2& vec) -> std::string {
    auto [x, y] = vec.properties();
    return std::format("x = {}, y = {}", x, y);
}

auto main() -> int {
    Vector2 vec1(1.0f, 2.0f);
    std::cout << "vec1: " << vec_str(vec1) << std::endl;
    Vector2 vec2(3.0f, 4.0f);
    std::cout << "vec2: " << vec_str(vec2) << std::endl;

    Vector2 vec3 = vec1 + vec2;
    std::cout << "vec1 + vec2: " << vec_str(vec3) << std::endl;
    Vector2 vec4 = vec1 - vec2;
    std::cout << "vec1 - vec2: " << vec_str(vec4) << std::endl;
    Vector2 vec5 = vec1 * 10;
    std::cout << "vec1 * 10: " << vec_str(vec5) << std::endl;
    Vector2 vec6 = vec1 / 10;
    std::cout << "vec1 / 10: " << vec_str(vec6) << std::endl;

    std::cout << "vec1.length(): " << vec1.length() << std::endl;
    vec1.normalization();
    std::cout << "vec1.normalization(): " << vec_str(vec1) << std::endl;
    std::cout << "vec1.length() after normalization(): " << vec1.length() << std::endl;
}