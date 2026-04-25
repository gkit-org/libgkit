#pragma once

#include "gkit/math/vector2.hpp"

#include <cmath>
#include <tuple>


namespace gkit::math {

    /// @brief 3D vector (x, y, z)
    class Vector3 {
    public:
        float x = 0.0f;
        float y = 0.0f;
        float z = 0.0f;

    public: // Constructors
        /// @brief Default constructor (uninitialized)
        Vector3() noexcept = default;

        /// @brief Create vector with uniform components
        /// @param v Value for all components
        explicit Vector3(float v) noexcept;

        /// @brief Create vector from components
        /// @param x X component
        /// @param y Y component
        /// @param z Z component
        Vector3(float x, float y, float z) noexcept;

        /// @brief Create from Vector2 with Z component
        /// @param v Vector2
        /// @param z Z component
        Vector3(const Vector2& v, float z) noexcept;

        /// @brief Copy constructor
        Vector3(const Vector3& other) noexcept = default;

        /// @brief Move constructor
        Vector3(const Vector3&& other) noexcept;

        /// @brief Destructor
        ~Vector3() noexcept = default;

    public: // Arithmetic operators
        /// @brief Assignment operator
        inline auto operator=(const Vector3& other) noexcept -> Vector3& = default;

        /// @brief Equality operator
        inline auto operator==(const Vector3& other) noexcept -> bool { return (this->x == other.x) && (this->y == other.y) && (this->z == other.z); }

        /// @brief Inequality operator
        inline auto operator!=(const Vector3& other) noexcept -> bool { return (this->x != other.x) || (this->y != other.y) || (this->z != other.z); }

        /// @brief Addition operator
        inline auto operator+(const Vector3& other) const noexcept -> Vector3 { return Vector3(this->x + other.x, this->y + other.y, this->z + other.z); }

        /// @brief Subtraction operator
        inline auto operator-(const Vector3& other) const noexcept -> Vector3 { return Vector3(this->x - other.x, this->y - other.y, this->z - other.z); }

        /// @brief Compound addition
        inline auto operator+=(const Vector3& other) noexcept -> const Vector3& { this->x += other.x; this->y += other.y; this->z += other.z; return *this; }

        /// @brief Compound subtraction
        inline auto operator-=(const Vector3& other) noexcept -> const Vector3& { this->x -= other.x; this->y -= other.y; this->z -= other.z; return *this; }

        /// @brief Scalar multiplication
        inline auto operator*(float s) noexcept -> Vector3 { return {this->x * s, this->y * s, this->z * s}; }

        /// @brief Scalar division
        inline auto operator/(float s) noexcept -> Vector3 { return {this->x / s, this->y / s, this->z / s}; }

        /// @brief Compound scalar multiplication
        inline auto operator*=(float s) noexcept -> const Vector3& { this->x *= s; this->y *= s; this->z *= s; return *this; }

        /// @brief Compound scalar division
        inline auto operator/=(float s) noexcept -> const Vector3& { this->x /= s; this->y /= s; this->z /= s; return *this; }

        /// @brief Negation operator
        inline auto operator-() noexcept -> Vector3 { return {-this->x, -this->y, -this->z}; }

    public: // Properties
        /// @brief Zero vector (0, 0, 0)
        inline static auto zero() noexcept -> Vector3 { return {0.0f, 0.0f, 0.0f}; }

        /// @brief One vector (1, 1, 1)
        inline static auto one() noexcept -> Vector3 { return {1.0f, 1.0f, 1.0f}; }

        /// @brief Vector length (magnitude)
        [[nodiscard]] inline auto length() const -> float { return std::sqrt(x * x + y * y + z * z); }

        /// @brief Squared length (avoids sqrt)
        [[nodiscard]] inline constexpr auto length_sq() const -> float { return x * x + y * y + z * z; }

        /// @brief Get all components as tuple
        [[nodiscard]] inline auto properties() const -> auto { return std::tie(x, y, z); }

        /// @brief Get all components as tuple (mutable)
        [[nodiscard]] inline auto properties() -> auto { return std::tie(x, y, z); }

        /// @brief Normalize to unit vector
        [[nodiscard]] inline auto normalize() const noexcept -> Vector3 {
            float len = this->length();
            return (len > NORMALIZE_TOLERANCE_32) ? Vector3{x / len, y / len, z / len} : Vector3{0.0f, 0.0f, 0.0f};
        }

    public: // Operations
        /// @brief Dot product
        /// @param a First vector
        /// @param b Second vector
        static inline auto dot(const Vector3& a, const Vector3& b) noexcept -> float { return a.x * b.x + a.y * b.y + a.z * b.z; }

        /// @brief Cross product
        /// @param a First vector
        /// @param b Second vector
        static inline auto cross(const Vector3& a, const Vector3& b) noexcept -> Vector3 {
            return {
                a.y * b.z - a.z * b.y,
                a.z * b.x - a.x * b.z,
                a.x * b.y - a.y * b.x
            };
        }

        /// @brief Linear interpolation
        /// @param a First vector
        /// @param b Second vector
        /// @param t Interpolation parameter (0 = a, 1 = b)
        static inline auto lerp(const Vector3& a, const Vector3& b, float t) noexcept -> Vector3 {
            return {a.x + t * (b.x - a.x), a.y + t * (b.y - a.y), a.z + t * (b.z - a.z)};
        }

        /// @brief Component-wise minimum
        static inline auto min(const Vector3& a, const Vector3& b) noexcept -> Vector3 {
            return {(a.x < b.x) ? a.x : b.x, (a.y < b.y) ? a.y : b.y, (a.z < b.z) ? a.z : b.z};
        }

        /// @brief Component-wise maximum
        static inline auto max(const Vector3& a, const Vector3& b) noexcept -> Vector3 {
            return {(a.x > b.x) ? a.x : b.x, (a.y > b.y) ? a.y : b.y, (a.z > b.z) ? a.z : b.z};
        }

        /// @brief Reflect vector about normal
        /// @param v Incident vector
        /// @param n Normal vector (must be normalized)
        static inline auto reflect(const Vector3& v, const Vector3& n) noexcept -> Vector3 {
            float d = 2.0f * dot(v, n);
            return {v.x - d * n.x, v.y - d * n.y, v.z - d * n.z};
        }

        /// @brief Euclidean distance between two points
        /// @param a First point
        /// @param b Second point
        static inline auto distance(const Vector3& a, const Vector3& b) noexcept -> float {
            float dx = b.x - a.x;
            float dy = b.y - a.y;
            float dz = b.z - a.z;
            return std::sqrt(dx * dx + dy * dy + dz * dz);
        }

        /// @brief Angle between two vectors (radians)
        /// @param a First vector
        /// @param b Second vector
        static inline auto angle(const Vector3& a, const Vector3& b) noexcept -> float {
            float d = Vector3::dot(a, b);
            float len_a = a.length();
            float len_b = b.length();
            return (len_a * len_b > 0.0f) ? std::acos(d / (len_a * len_b)) : 0.0f;
        }

        /// @brief Component-wise multiplication (Hadamard product)
        /// @param a First vector
        /// @param b Second vector
        static inline auto component_mul(const Vector3& a, const Vector3& b) noexcept -> Vector3 {
            return {a.x * b.x, a.y * b.y, a.z * b.z};
        }
    }; // class Vector3
} // namespace gkit::math
