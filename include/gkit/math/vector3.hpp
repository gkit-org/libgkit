#pragma once

#include "gkit/math/vector2.hpp"

#include <cmath>
#include <tuple>


namespace gkit::math {
    class Vector3 {
    public:
        float x = 0.0f;
        float y = 0.0f;
        float z = 0.0f;

        Vector3() noexcept = default;
        explicit Vector3(float v) noexcept;
        Vector3(float x, float y, float z) noexcept;
        Vector3(const Vector2& v, float z) noexcept;
        Vector3(const Vector3& other)  noexcept = default;
        Vector3(const Vector3&& other) noexcept;
        ~Vector3() noexcept = default;

    public: // Arithmetic operators
        inline auto operator=(const Vector3& other) noexcept -> Vector3& = default;
        inline auto operator==(const Vector3& other) noexcept -> bool { return (this->x == other.x) && (this->y == other.y) && (this->z == other.z); }
        inline auto operator!=(const Vector3& other) noexcept -> bool { return (this->x != other.x) || (this->y != other.y) || (this->z != other.z); }
        inline auto operator+(const Vector3& other) noexcept -> Vector3 { return Vector3(this->x + other.x, this->y + other.y, this->z + other.z); }
        inline auto operator-(const Vector3& other) noexcept -> Vector3 { return Vector3(this->x - other.x, this->y - other.y, this->z - other.z); }
        inline auto operator+=(const Vector3& other) noexcept -> const Vector3& { this->x += other.x; this->y += other.y; this->z += other.z; return *this; }
        inline auto operator-=(const Vector3& other) noexcept -> const Vector3& { this->x -= other.x; this->y -= other.y; this->z -= other.z; return *this; }
        inline auto operator*(float s) noexcept -> Vector3 { return {this->x * s, this->y * s, this->z * s}; }
        inline auto operator/(float s) noexcept -> Vector3 { return {this->x / s, this->y / s, this->z / s}; }
        inline auto operator*=(float s) noexcept -> const Vector3& { this->x *= s; this->y *= s; this->z *= s; return *this; }
        inline auto operator/=(float s) noexcept -> const Vector3& { this->x /= s; this->y /= s; this->z /= s; return *this; }
        inline auto operator-() noexcept -> Vector3 { return {-this->x, -this->y, -this->z}; }

    public: // Properties
        inline static auto zero() noexcept -> Vector3 { return {0.0f, 0.0f, 0.0f}; }
        inline static auto one() noexcept -> Vector3 { return {1.0f, 1.0f, 1.0f}; }

        [[nodiscard]] inline auto length() const -> float { return std::sqrt(x * x + y * y + z * z); }
        [[nodiscard]] inline constexpr auto length_sq() const -> float { return x * x + y * y + z * z; }
        [[nodiscard]] inline auto properties() const -> auto { return std::tie(x, y, z); }
        [[nodiscard]] inline auto properties() -> auto { return std::tie(x, y, z); }
        [[nodiscard]] inline auto normalize() noexcept -> Vector3 {
            float len = this->length();
            return (len > 1e-6f) ? Vector3{x / len, y / len, z / len} : Vector3{0.0f, 0.0f, 0.0f};
        }
    public: // Operations
        static inline auto dot(const Vector3& a, const Vector3& b) noexcept -> float { return a.x * b.x + a.y * b.y + a.z * b.z; }
        static inline auto cross(const Vector3& a, const Vector3& b) noexcept -> Vector3 {
            return {
                a.y * b.z - a.z * b.y,
                a.z * b.x - a.x * b.z,
                a.x * b.y - a.y * b.x
            };
        }

        static inline auto lerp(const Vector3& a, const Vector3& b, float t) noexcept -> Vector3 {
            return {a.x + t * (b.x - a.x), a.y + t * (b.y - a.y), a.z + t * (b.z - a.z)};
        }
        static inline auto min(const Vector3& a, const Vector3& b) noexcept -> Vector3 {
            return {(a.x < b.x) ? a.x : b.x, (a.y < b.y) ? a.y : b.y, (a.z < b.z) ? a.z : b.z};
        }
        static inline auto max(const Vector3& a, const Vector3& b) noexcept -> Vector3 {
            return {(a.x > b.x) ? a.x : b.x, (a.y > b.y) ? a.y : b.y, (a.z > b.z) ? a.z : b.z};
        }
        static inline auto reflect(const Vector3& v, const Vector3& n) noexcept -> Vector3 {
            float d = 2.0f * dot(v, n);
            return {v.x - d * n.x, v.y - d * n.y, v.z - d * n.z};
        }
        static inline auto distance(const Vector3& a, const Vector3& b) noexcept -> float {
            float dx = b.x - a.x;
            float dy = b.y - a.y;
            float dz = b.z - a.z;
            return std::sqrt(dx * dx + dy * dy + dz * dz);
        }
        static inline auto angle(const Vector3& a, const Vector3& b) noexcept -> float {
            float d = Vector3::dot(a, b);
            float len_a = a.length();
            float len_b = b.length();
            return (len_a * len_b > 0.0f) ? std::acos(d / (len_a * len_b)) : 0.0f;
        }
        static inline auto component_mul(const Vector3& a, const Vector3& b) noexcept -> Vector3 {
            return {a.x * b.x, a.y * b.y, a.z * b.z};
        }
    }; // class Vector3
} // namespace gkit::math