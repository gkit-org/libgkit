#pragma once

#include "gkit/math/vector3.hpp"

#include <cmath>
#include <tuple>


namespace gkit::math {
    class Vector4 {
    public:
        float x = 0.0f;
        float y = 0.0f;
        float z = 0.0f;
        float w = 0.0f;

        Vector4() noexcept = default;
        explicit Vector4(float v) noexcept;
        Vector4(float x, float y, float z, float w) noexcept;
        Vector4(const Vector3& v, float w) noexcept;
        Vector4(const Vector2& v, float z, float w) noexcept;
        Vector4(const Vector4& other)  noexcept = default;
        Vector4(const Vector4&& other) noexcept;
        ~Vector4() noexcept = default;

    public: // Arithmetic operators
        inline auto operator=(const Vector4& other) noexcept -> Vector4& = default;
        inline auto operator==(const Vector4& other) noexcept -> bool { return (this->x == other.x) && (this->y == other.y) && (this->z == other.z) && (this->w == other.w); }
        inline auto operator!=(const Vector4& other) noexcept -> bool { return (this->x != other.x) || (this->y != other.y) || (this->z != other.z) || (this->w != other.w); }
        inline auto operator+(const Vector4& other) noexcept -> Vector4 { return Vector4(this->x + other.x, this->y + other.y, this->z + other.z, this->w + other.w); }
        inline auto operator-(const Vector4& other) noexcept -> Vector4 { return Vector4(this->x - other.x, this->y - other.y, this->z - other.z, this->w - other.w); }
        inline auto operator+=(const Vector4& other) noexcept -> const Vector4& { this->x += other.x; this->y += other.y; this->z += other.z; this->w += other.w; return *this; }
        inline auto operator-=(const Vector4& other) noexcept -> const Vector4& { this->x -= other.x; this->y -= other.y; this->z -= other.z; this->w -= other.w; return *this; }
        inline auto operator*(float s) noexcept -> Vector4 { return {this->x * s, this->y * s, this->z * s, this->w * s}; }
        inline auto operator/(float s) noexcept -> Vector4 { return {this->x / s, this->y / s, this->z / s, this->w / s}; }
        inline auto operator*=(float s) noexcept -> const Vector4& { this->x *= s; this->y *= s; this->z *= s; this->w *= s; return *this; }
        inline auto operator/=(float s) noexcept -> const Vector4& { this->x /= s; this->y /= s; this->z /= s; this->w /= s; return *this; }
        inline auto operator-() noexcept -> Vector4 { return {-this->x, -this->y, -this->z, -this->w}; }

    public: // Properties
        inline static auto zero() noexcept -> Vector4 { return {0.0f, 0.0f, 0.0f, 0.0f}; }
        inline static auto one() noexcept -> Vector4 { return {1.0f, 1.0f, 1.0f, 1.0f}; }

        [[nodiscard]] inline auto length() const -> float { return std::sqrt(x * x + y * y + z * z + w * w); }
        [[nodiscard]] inline constexpr auto length_sq() const -> float { return x * x + y * y + z * z + w * w; }
        [[nodiscard]] inline auto properties() const -> auto { return std::tie(x, y, z, w); }
        [[nodiscard]] inline auto properties() -> auto { return std::tie(x, y, z, w); }
        [[nodiscard]] inline auto normalize() noexcept -> Vector4 {
            float len = this->length();
            return (len > 1e-8f) ? Vector4{x / len, y / len, z / len, w / len} : Vector4{0.0f, 0.0f, 0.0f, 0.0f};
        }
    public: // Operations
        static inline auto dot(const Vector4& a, const Vector4& b) noexcept -> float { return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w; }

        static inline auto lerp(const Vector4& a, const Vector4& b, float t) noexcept -> Vector4 {
            return {a.x + t * (b.x - a.x), a.y + t * (b.y - a.y), a.z + t * (b.z - a.z), a.w + t * (b.w - a.w)};
        }
        static inline auto min(const Vector4& a, const Vector4& b) noexcept -> Vector4 {
            return {(a.x < b.x) ? a.x : b.x, (a.y < b.y) ? a.y : b.y, (a.z < b.z) ? a.z : b.z, (a.w < b.w) ? a.w : b.w};
        }
        static inline auto max(const Vector4& a, const Vector4& b) noexcept -> Vector4 {
            return {(a.x > b.x) ? a.x : b.x, (a.y > b.y) ? a.y : b.y, (a.z > b.z) ? a.z : b.z, (a.w > b.w) ? a.w : b.w};
        }
        static inline auto distance(const Vector4& a, const Vector4& b) noexcept -> float {
            float dx = b.x - a.x;
            float dy = b.y - a.y;
            float dz = b.z - a.z;
            float dw = b.w - a.w;
            return std::sqrt(dx * dx + dy * dy + dz * dz + dw * dw);
        }
        static inline auto component_mul(const Vector4& a, const Vector4& b) noexcept -> Vector4 {
            return {a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w};
        }
    }; // class Vector4
} // namespace gkit::math