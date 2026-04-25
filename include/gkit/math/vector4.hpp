#pragma once

#include "gkit/math/vector3.hpp"

#include <cmath>
#include <tuple>


namespace gkit::math {

    /// @brief 4D vector (x, y, z, w)
    class Vector4 {
    public:
        float x = 0.0f;
        float y = 0.0f;
        float z = 0.0f;
        float w = 0.0f;

    public: // Constructors
        /// @brief Default constructor (uninitialized)
        Vector4() noexcept = default;

        /// @brief Create vector with uniform components
        /// @param v Value for all components
        explicit Vector4(float v) noexcept;

        /// @brief Create vector from components
        /// @param x X component
        /// @param y Y component
        /// @param z Z component
        /// @param w W component
        Vector4(float x, float y, float z, float w) noexcept;

        /// @brief Create from Vector3 with W component
        /// @param v Vector3
        /// @param w W component
        Vector4(const Vector3& v, float w) noexcept;

        /// @brief Create from Vector2 with Z and W components
        /// @param v Vector2
        /// @param z Z component
        /// @param w W component
        Vector4(const Vector2& v, float z, float w) noexcept;

        /// @brief Copy constructor
        Vector4(const Vector4& other) noexcept = default;

        /// @brief Move constructor
        Vector4(const Vector4&& other) noexcept;

        /// @brief Destructor
        ~Vector4() noexcept = default;

    public: // Arithmetic operators
        /// @brief Assignment operator
        inline auto operator=(const Vector4& other) noexcept -> Vector4& = default;

        /// @brief Equality operator
        inline auto operator==(const Vector4& other) noexcept -> bool { return (this->x == other.x) && (this->y == other.y) && (this->z == other.z) && (this->w == other.w); }

        /// @brief Inequality operator
        inline auto operator!=(const Vector4& other) noexcept -> bool { return (this->x != other.x) || (this->y != other.y) || (this->z != other.z) || (this->w != other.w); }

        /// @brief Addition operator
        inline auto operator+(const Vector4& other) const noexcept -> Vector4 { return Vector4(this->x + other.x, this->y + other.y, this->z + other.z, this->w + other.w); }

        /// @brief Subtraction operator
        inline auto operator-(const Vector4& other) const noexcept -> Vector4 { return Vector4(this->x - other.x, this->y - other.y, this->z - other.z, this->w - other.w); }

        /// @brief Compound addition
        inline auto operator+=(const Vector4& other) noexcept -> const Vector4& { this->x += other.x; this->y += other.y; this->z += other.z; this->w += other.w; return *this; }

        /// @brief Compound subtraction
        inline auto operator-=(const Vector4& other) noexcept -> const Vector4& { this->x -= other.x; this->y -= other.y; this->z -= other.z; this->w -= other.w; return *this; }

        /// @brief Scalar multiplication
        inline auto operator*(float s) noexcept -> Vector4 { return {this->x * s, this->y * s, this->z * s, this->w * s}; }

        /// @brief Scalar division
        inline auto operator/(float s) noexcept -> Vector4 { return {this->x / s, this->y / s, this->z / s, this->w / s}; }

        /// @brief Compound scalar multiplication
        inline auto operator*=(float s) noexcept -> const Vector4& { this->x *= s; this->y *= s; this->z *= s; this->w *= s; return *this; }

        /// @brief Compound scalar division
        inline auto operator/=(float s) noexcept -> const Vector4& { this->x /= s; this->y /= s; this->z /= s; this->w /= s; return *this; }

        /// @brief Negation operator
        inline auto operator-() noexcept -> Vector4 { return {-this->x, -this->y, -this->z, -this->w}; }

    public: // Properties
        /// @brief Zero vector (0, 0, 0, 0)
        inline static auto zero() noexcept -> Vector4 { return {0.0f, 0.0f, 0.0f, 0.0f}; }

        /// @brief One vector (1, 1, 1, 1)
        inline static auto one() noexcept -> Vector4 { return {1.0f, 1.0f, 1.0f, 1.0f}; }

        /// @brief Vector length (magnitude)
        [[nodiscard]] inline auto length() const -> float { return std::sqrt(x * x + y * y + z * z + w * w); }

        /// @brief Squared length (avoids sqrt)
        [[nodiscard]] inline constexpr auto length_sq() const -> float { return x * x + y * y + z * z + w * w; }

        /// @brief Get all components as tuple
        [[nodiscard]] inline auto properties() const -> auto { return std::tie(x, y, z, w); }

        /// @brief Get all components as tuple (mutable)
        [[nodiscard]] inline auto properties() -> auto { return std::tie(x, y, z, w); }

        /// @brief Normalize to unit vector
        [[nodiscard]] inline auto normalize() const noexcept -> Vector4 {
            float len = this->length();
            return (len > NORMALIZE_TOLERANCE_32) ? Vector4{x / len, y / len, z / len, w / len} : Vector4{0.0f, 0.0f, 0.0f, 0.0f};
        }

    public: // Operations
        /// @brief Dot product
        /// @param a First vector
        /// @param b Second vector
        static inline auto dot(const Vector4& a, const Vector4& b) noexcept -> float { return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w; }

        /// @brief Linear interpolation
        /// @param a First vector
        /// @param b Second vector
        /// @param t Interpolation parameter (0 = a, 1 = b)
        static inline auto lerp(const Vector4& a, const Vector4& b, float t) noexcept -> Vector4 {
            return {a.x + t * (b.x - a.x), a.y + t * (b.y - a.y), a.z + t * (b.z - a.z), a.w + t * (b.w - a.w)};
        }

        /// @brief Component-wise minimum
        static inline auto min(const Vector4& a, const Vector4& b) noexcept -> Vector4 {
            return {(a.x < b.x) ? a.x : b.x, (a.y < b.y) ? a.y : b.y, (a.z < b.z) ? a.z : b.z, (a.w < b.w) ? a.w : b.w};
        }

        /// @brief Component-wise maximum
        static inline auto max(const Vector4& a, const Vector4& b) noexcept -> Vector4 {
            return {(a.x > b.x) ? a.x : b.x, (a.y > b.y) ? a.y : b.y, (a.z > b.z) ? a.z : b.z, (a.w > b.w) ? a.w : b.w};
        }

        /// @brief Euclidean distance between two points
        /// @param a First point
        /// @param b Second point
        static inline auto distance(const Vector4& a, const Vector4& b) noexcept -> float {
            float dx = b.x - a.x;
            float dy = b.y - a.y;
            float dz = b.z - a.z;
            float dw = b.w - a.w;
            return std::sqrt(dx * dx + dy * dy + dz * dz + dw * dw);
        }

        /// @brief Component-wise multiplication (Hadamard product)
        /// @param a First vector
        /// @param b Second vector
        static inline auto component_mul(const Vector4& a, const Vector4& b) noexcept -> Vector4 {
            return {a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w};
        }
    }; // class Vector4
} // namespace gkit::math
