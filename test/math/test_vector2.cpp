#include "gkit/math/vector2.hpp"
#include "gkit/math/scalar.hpp"

#include <cassert>
#include <cmath>
#include <iostream>
#include <format>
#include <string>


using gkit::math::Vector2;

auto vec_str(const Vector2& vec) -> std::string {
    return std::format("x = {:.4f}, y = {:.4f}", vec.x, vec.y);
}

auto main() -> int {
    // Basic constructors
    Vector2 vec1(1.0f, 2.0f);
    std::cout << "vec1: " << vec_str(vec1) << std::endl;

    // Fill constructor
    Vector2 vec_fill(5.0f);
    assert(vec_fill.x == 5.0f && vec_fill.y == 5.0f);
    std::cout << "vec_fill (Vector2(5.0f)): " << vec_str(vec_fill) << std::endl;

    Vector2 vec2(3.0f, 4.0f);
    std::cout << "vec2: " << vec_str(vec2) << std::endl;

    // Arithmetic operators
    Vector2 vec_add = vec1 + vec2;
    std::cout << "vec1 + vec2: " << vec_str(vec_add) << std::endl;
    assert(vec_add.x == 4.0f && vec_add.y == 6.0f);

    Vector2 vec_sub = vec1 - vec2;
    std::cout << "vec1 - vec2: " << vec_str(vec_sub) << std::endl;
    assert(vec_sub.x == -2.0f && vec_sub.y == -2.0f);

    Vector2 vec_mul = vec1 * 10.0f;
    std::cout << "vec1 * 10: " << vec_str(vec_mul) << std::endl;
    assert(vec_mul.x == 10.0f && vec_mul.y == 20.0f);

    Vector2 vec_div = vec1 / 10.0f;
    std::cout << "vec1 / 10: " << vec_str(vec_div) << std::endl;
    assert(std::abs(vec_div.x - 0.1f) < gkit::math::fp::EPSILON32 && std::abs(vec_div.y - 0.2f) < gkit::math::fp::EPSILON32);

    // Length
    std::cout << "vec1.length(): " << vec1.length() << std::endl;
    assert(std::abs(vec1.length() - std::sqrt(5.0f)) < gkit::math::fp::EPSILON32);

    // Length squared
    std::cout << "vec1.length_sq(): " << vec1.length_sq() << std::endl;
    assert(std::abs(vec1.length_sq() - 5.0f) < gkit::math::fp::EPSILON32);

    // Normalization
    vec1 = Vector2::normalize(vec1);
    std::cout << "vec1 after normalize: " << vec_str(vec1) << std::endl;
    assert(std::abs(vec1.length() - 1.0f) < gkit::math::fp::EPSILON32);

    // Reset for further tests
    Vector2 a(1.0f, 0.0f);
    Vector2 b(0.0f, 1.0f);

    // Dot product
    float dot_result = Vector2::dot(a, b);
    std::cout << "Vector2::dot(a, b): " << dot_result << std::endl;
    assert(dot_result == 0.0f);

    float dot_a = Vector2::dot(a, a);
    assert(dot_a == 1.0f);

    // Cross product
    float cross_result = Vector2::cross(a, b);
    std::cout << "Vector2::cross(a, b): " << cross_result << std::endl;
    assert(cross_result == 1.0f);

    // Normalize (static)
    Vector2 c(3.0f, 4.0f);
    Vector2 c_normalized = Vector2::normalize(c);
    std::cout << "Vector2::normalize(3,4): " << vec_str(c_normalized) << std::endl;
    assert(std::abs(c_normalized.length() - 1.0f) < gkit::math::fp::EPSILON32);

    // Lerp
    Vector2 lerp_result = Vector2::lerp(a, b, 0.5f);
    std::cout << "Vector2::lerp((1,0), (0,1), 0.5): " << vec_str(lerp_result) << std::endl;
    assert(std::abs(lerp_result.x - 0.5f) < gkit::math::fp::EPSILON32);
    assert(std::abs(lerp_result.y - 0.5f) < gkit::math::fp::EPSILON32);

    // Min / Max
    Vector2 min_result = Vector2::min(a, b);
    std::cout << "Vector2::min((1,0), (0,1)): " << vec_str(min_result) << std::endl;
    assert(min_result.x == 0.0f && min_result.y == 0.0f);

    Vector2 max_result = Vector2::max(a, b);
    std::cout << "Vector2::max((1,0), (0,1)): " << vec_str(max_result) << std::endl;
    assert(max_result.x == 1.0f && max_result.y == 1.0f);

    // Perp
    Vector2 perp_result = Vector2::perp(a);
    std::cout << "Vector2::perp((1,0)): " << vec_str(perp_result) << std::endl;
    assert(perp_result.x == 0.0f && perp_result.y == 1.0f);

    // Reflect
    Vector2 v(1.0f, 1.0f);
    Vector2 n(0.0f, 1.0f);  // Reflect off horizontal surface
    Vector2 reflect_result = Vector2::reflect(v, n);
    std::cout << "Vector2::reflect((1,1), (0,1)): " << vec_str(reflect_result) << std::endl;
    assert(std::abs(reflect_result.x - 1.0f) < gkit::math::fp::EPSILON32);
    assert(std::abs(reflect_result.y - (-1.0f)) < gkit::math::fp::EPSILON32);

    // Distance
    Vector2 p1(0.0f, 0.0f);
    Vector2 p2(3.0f, 4.0f);
    float dist = Vector2::distance(p1, p2);
    std::cout << "Vector2::distance((0,0), (3,4)): " << dist << std::endl;
    assert(std::abs(dist - 5.0f) < gkit::math::fp::EPSILON32);

    std::cout << "\nAll tests passed!" << std::endl;
    return 0;
}