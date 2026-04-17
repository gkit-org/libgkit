#pragma once

#include <cmath>
#include <tuple>


namespace gkit::math {
    class Vector2 final {
    public:
        float x = 0.0f;
        float y = 0.0f;

        Vector2() noexcept = default;
        explicit Vector2(float v) noexcept;
        Vector2(float x, float y) noexcept;
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
        inline auto operator*(float s) noexcept -> Vector2 { return {this->x * s, this->y * s}; }
        inline auto operator/(float s) noexcept -> Vector2 { return {this->x / s, this->y / s}; }
        inline auto operator*=(float s) noexcept -> const Vector2& { this->x *= s; this->y *= s; return *this; }
        inline auto operator/=(float s) noexcept -> const Vector2& { this->x /= s; this->y /= s; return *this; }
        inline auto operator-() noexcept -> Vector2 { return {-this->x, -this->y}; }

    public: // Properties
        [[nodiscard]] inline auto length() const -> float { return std::sqrt(x * x + y * y); }
        [[nodiscard]] inline constexpr auto length_sq() const -> float { return x * x + y * y; }
        [[nodiscard]] inline auto properties() const -> auto { return std::tie(x, y); }
        [[nodiscard]] inline auto properties() -> auto { return std::tie(x, y); }
        [[nodiscard]] inline auto normalize() noexcept -> Vector2 {
            float len = this->length();
            return (len > 0.0f) ? Vector2{x / len, y / len} : Vector2{0.0f, 0.0f};
        }
    public: // Operations
        inline static auto zero() noexcept -> Vector2 { return {0.0f, 0.0f}; }
        inline static auto one() noexcept -> Vector2 { return {1.0f, 1.0f}; }


        static inline auto dot(const Vector2& a, const Vector2& b) noexcept -> float { return a.x * b.x + a.y * b.y; }
        static inline auto cross(const Vector2& a, const Vector2& b) noexcept -> float { return a.x * b.y - a.y * b.x; }

        static inline auto lerp(const Vector2& a, const Vector2& b, float t) noexcept -> Vector2 {
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
            float d = 2.0f * dot(v, n);
            return {v.x - d * n.x, v.y - d * n.y};
        }
        static inline auto distance(const Vector2& a, const Vector2& b) noexcept -> float {
            float dx = b.x - a.x;
            float dy = b.y - a.y;
            return std::sqrt(dx * dx + dy * dy);
        }
    }; // class Vector2
} // namespace gkit::math