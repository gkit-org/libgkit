#pragma once

#include "gkit/math/scalar.hpp"

#include <cmath>
#include <tuple>


namespace gkit::math {
    class Vector2 final {
    public:
        float32 x = 0.0f;
        float32 y = 0.0f;

        Vector2() noexcept = default;
        explicit Vector2(float32 v) noexcept;
        Vector2(float32 x, float32 y) noexcept;
        Vector2(const Vector2& other)  noexcept;
        Vector2(const Vector2&& other) noexcept;
        ~Vector2() noexcept = default;

    public: // Arithmetic operators
        inline auto operator=(const Vector2& other) noexcept -> Vector2& { this->x = other.x; this->y = other.y; return *this; }
        inline auto operator=(const Vector2&& other) noexcept -> Vector2& { this->x = other.x; this->y = other.y; return *this; }
        inline auto operator==(const Vector2& other) noexcept -> bool { return this->x == other.x && this->y == other.y; }
        inline auto operator!=(const Vector2& other) noexcept -> bool { return this->x != other.x || this->y != other.y; }
        inline auto operator+(const Vector2& other) noexcept -> Vector2 { return Vector2(this->x + other.x, this->y + other.y); }
        inline auto operator-(const Vector2& other) noexcept -> Vector2 { return Vector2(this->x - other.x, this->y - other.y); }
        inline auto operator+=(const Vector2& other) noexcept -> const Vector2& { this->x += other.x; this->y += other.y; return *this; }
        inline auto operator-=(const Vector2& other) noexcept -> const Vector2& { this->x -= other.x; this->y -= other.y; return *this; }
        inline auto operator*(float32 s) noexcept -> Vector2 { return {this->x * s, this->y * s}; }
        inline auto operator/(float32 s) noexcept -> Vector2 { return {this->x / s, this->y / s}; }
        inline auto operator*=(float32 s) noexcept -> const Vector2& { this->x *= s; this->y *= s; return *this; }
        inline auto operator/=(float32 s) noexcept -> const Vector2& { this->x /= s; this->y /= s; return *this; }
        inline auto operator-() noexcept -> Vector2 { return {-this->x, -this->y}; }

    public: // Properties
        [[nodiscard]] inline auto length() const -> float32 { return std::sqrt(x * x + y * y); }
        [[nodiscard]] inline constexpr auto length_sq() const -> float32 { return x * x + y * y; }
        inline auto properties() -> auto { return std::tie(x, y); }

    public: // Operations
        inline static auto zero() noexcept -> Vector2 { return {0.0f, 0.0f}; }
        inline static auto one() noexcept -> Vector2 { return {1.0f, 1.0f}; }


        static inline auto dot(const Vector2& a, const Vector2& b) noexcept -> float32 { return a.x * b.x + a.y * b.y; }
        static inline auto cross(const Vector2& a, const Vector2& b) noexcept -> float32 { return a.x * b.y - a.y * b.x; }
        static inline auto normalize(const Vector2& v) noexcept -> Vector2 {
            float32 len = v.length();
            return (len > 0.0f) ? Vector2{v.x / len, v.y / len} : Vector2{0.0f, 0.0f};
        }
        static inline auto lerp(const Vector2& a, const Vector2& b, float32 t) noexcept -> Vector2 {
            return {a.x + t * (b.x - a.x), a.y + t * (b.y - a.y)};
        }
        static inline auto min(const Vector2& a, const Vector2& b) noexcept -> Vector2 {
            return {(a.x < b.x) ? a.x : b.x, (a.y < b.y) ? a.y : b.y};
        }
        static inline auto max(const Vector2& a, const Vector2& b) noexcept -> Vector2 {
            return {(a.x > b.x) ? a.x : b.x, (a.y > b.y) ? a.y : b.y};
        }
        static inline auto perp(const Vector2& v) noexcept -> Vector2 { return {-v.y, v.x}; }
        static inline auto reflect(const Vector2& v, const Vector2& n) noexcept -> Vector2 {
            float32 d = 2.0f * dot(v, n);
            return {v.x - d * n.x, v.y - d * n.y};
        }
        static inline auto distance(const Vector2& a, const Vector2& b) noexcept -> float32 {
            float32 dx = b.x - a.x;
            float32 dy = b.y - a.y;
            return std::sqrt(dx * dx + dy * dy);
        }
    }; // class Vector2
} // namespace gkit::math