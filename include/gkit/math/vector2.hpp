#pragma once

#include "gkit/math/constants.hpp"

#include <cmath>
#include <tuple>


namespace gkit::math {

    /// @brief 2D vector (x, y)
    class Vector2 final {
    public:
        float x = 0.0f;
        float y = 0.0f;

    public: // Constructors
        /// @brief Default constructor (uninitialized)
        Vector2() noexcept = default;

        /// @brief Create vector with uniform components
        /// @param v Value for all components
        explicit Vector2(float v) noexcept;

        /// @brief Create vector from components
        /// @param x X component
        /// @param y Y component
        Vector2(float x, float y) noexcept;

        /// @brief Copy constructor
        Vector2(const Vector2& other) noexcept;

        /// @brief Move constructor
        Vector2(const Vector2&& other) noexcept;

        /// @brief Destructor
        ~Vector2() noexcept = default;

    public: // Arithmetic operators
        /// @brief Assignment operator
        inline auto operator=(const Vector2& other) noexcept -> Vector2& { this->x = other.x; this->y = other.y; return *this; }

        /// @brief Move assignment operator
        inline auto operator=(const Vector2&& other) noexcept -> Vector2& { this->x = other.x; this->y = other.y; return *this; }

        /// @brief Equality operator
        inline auto operator==(const Vector2& other) noexcept -> bool { return this->x == other.x && this->y == other.y; }

        /// @brief Inequality operator
        inline auto operator!=(const Vector2& other) noexcept -> bool { return this->x != other.x || this->y != other.y; }

        /// @brief Addition operator
        inline auto operator+(const Vector2& other) noexcept -> Vector2 { return Vector2(this->x + other.x, this->y + other.y); }

        /// @brief Subtraction operator
        inline auto operator-(const Vector2& other) noexcept -> Vector2 { return Vector2(this->x - other.x, this->y - other.y); }

        /// @brief Compound addition
        inline auto operator+=(const Vector2& other) noexcept -> const Vector2& { this->x += other.x; this->y += other.y; return *this; }

        /// @brief Compound subtraction
        inline auto operator-=(const Vector2& other) noexcept -> const Vector2& { this->x -= other.x; this->y -= other.y; return *this; }

        /// @brief Scalar multiplication
        inline auto operator*(float s) noexcept -> Vector2 { return {this->x * s, this->y * s}; }

        /// @brief Scalar division
        inline auto operator/(float s) noexcept -> Vector2 { return {this->x / s, this->y / s}; }

        /// @brief Compound scalar multiplication
        inline auto operator*=(float s) noexcept -> const Vector2& { this->x *= s; this->y *= s; return *this; }

        /// @brief Compound scalar division
        inline auto operator/=(float s) noexcept -> const Vector2& { this->x /= s; this->y /= s; return *this; }

        /// @brief Negation operator
        inline auto operator-() noexcept -> Vector2 { return {-this->x, -this->y}; }

    public: // Properties
        /// @brief Vector length (magnitude)
        [[nodiscard]] inline auto length() const -> float { return std::sqrt(x * x + y * y); }

        /// @brief Squared length (avoids sqrt)
        [[nodiscard]] inline constexpr auto length_sq() const -> float { return x * x + y * y; }

        /// @brief Get all components as tuple
        [[nodiscard]] inline auto properties() const -> auto { return std::tie(x, y); }

        /// @brief Get all components as tuple (mutable)
        [[nodiscard]] inline auto properties() -> auto { return std::tie(x, y); }

        /// @brief Normalize to unit vector
        [[nodiscard]] inline auto normalize() const noexcept -> Vector2 {
            float len = this->length();
            return (len > NORMALIZE_TOLERANCE_32) ? Vector2{x / len, y / len} : Vector2{0.0f, 0.0f};
        }

    public: // Operations
        /// @brief Zero vector (0, 0)
        inline static auto zero() noexcept -> Vector2 { return {0.0f, 0.0f}; }

        /// @brief One vector (1, 1)
        inline static auto one() noexcept -> Vector2 { return {1.0f, 1.0f}; }

        /// @brief Dot product
        /// @param a First vector
        /// @param b Second vector
        static inline auto dot(const Vector2& a, const Vector2& b) noexcept -> float { return a.x * b.x + a.y * b.y; }

        /// @brief 2D cross product (returns scalar)
        /// @param a First vector
        /// @param b Second vector
        static inline auto cross(const Vector2& a, const Vector2& b) noexcept -> float { return a.x * b.y - a.y * b.x; }

        /// @brief Linear interpolation
        /// @param a First vector
        /// @param b Second vector
        /// @param t Interpolation parameter (0 = a, 1 = b)
        static inline auto lerp(const Vector2& a, const Vector2& b, float t) noexcept -> Vector2 {
            return {a.x + t * (b.x - a.x), a.y + t * (b.y - a.y)};
        }

        /// @brief Component-wise minimum
        static inline auto min(const Vector2& a, const Vector2& b) noexcept -> Vector2 {
            return {(a.x < b.x) ? a.x : b.x, (a.y < b.y) ? a.y : b.y};
        }

        /// @brief Component-wise maximum
        static inline auto max(const Vector2& a, const Vector2& b) noexcept -> Vector2 {
            return {(a.x > b.x) ? a.x : b.x, (a.y > b.y) ? a.y : b.y};
        }

        /// @brief Perpendicular vector (rotated 90 degrees counter-clockwise)
        /// @param v Input vector
        static inline auto perp(const Vector2& v) noexcept -> Vector2 { return {-v.y, v.x}; }

        /// @brief Reflect vector about normal
        /// @param v Incident vector
        /// @param n Normal vector (must be normalized)
        static inline auto reflect(const Vector2& v, const Vector2& n) noexcept -> Vector2 {
            float d = 2.0f * dot(v, n);
            return {v.x - d * n.x, v.y - d * n.y};
        }

        /// @brief Euclidean distance between two points
        /// @param a First point
        /// @param b Second point
        static inline auto distance(const Vector2& a, const Vector2& b) noexcept -> float {
            float dx = b.x - a.x;
            float dy = b.y - a.y;
            return std::sqrt(dx * dx + dy * dy);
        }
    }; // class Vector2
} // namespace gkit::math
