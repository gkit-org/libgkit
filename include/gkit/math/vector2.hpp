#pragma once

#include <cmath>
#include <cstdint>
#include <tuple>

namespace gkit::math {
    class Vector2 final {
    public:
        Vector2() noexcept = default;
        Vector2(float x, float y) noexcept;
        Vector2(const Vector2& other)  noexcept;
        Vector2(const Vector2&& other) noexcept;
        ~Vector2() noexcept = default;

     public: // Arithmetic operators
        inline auto operator= (const Vector2& other) noexcept -> Vector2& { this->x = other.x; this->y = other.y; return *this; }
        inline auto operator= (const Vector2&&other) noexcept -> Vector2& { this->x = other.x; this->y = other.y; return *this; }
        inline auto operator==(const Vector2& other) noexcept -> bool { return this->x == other.x && this->y == other.y; }
        inline auto operator!=(const Vector2& other) noexcept -> bool { return this->x != other.x || this->y != other.y; }
        inline auto operator+ (const Vector2& other) noexcept -> Vector2 { return Vector2(this->x + other.x, this->y + other.y); }
        inline auto operator- (const Vector2& other) noexcept -> Vector2 { return Vector2(this->x - other.x, this->y - other.y); }
        inline auto operator+=(const Vector2& other) noexcept -> const Vector2& { this->x += other.x; this->y += other.y; return *this; }
        inline auto operator-=(const Vector2& other) noexcept -> const Vector2& { this->x -= other.x; this->y -= other.y; return *this; }
        inline auto operator* (const int32_t n) noexcept -> Vector2 { return Vector2(this->x * n, this->y * n); }
        inline auto operator/ (const int32_t n) noexcept -> Vector2 { return Vector2(this->x / n, this->y / n); }
        inline auto operator*=(const int32_t n) noexcept -> const Vector2& { this->x *= n; this->y *= n; return *this; }
        inline auto operator/=(const int32_t n) noexcept -> const Vector2& { this->x /= n; this->y /= n; return *this; }

    public:
        static auto zero() -> const Vector2&;
        auto normalization() -> void;
#ifdef _MSC_VER
        inline auto length() const -> float { return std::sqrt(x * x + y * y); }
#else
        inline constexpr auto length() const -> float { return std::sqrt(x * x + y * y); }
#endif
        inline auto properties() -> auto { return std::tie(x, y);}

    private:
        float x = 0.f,
              y = 0.f;
    }; // class Vector2
} // namespace math
