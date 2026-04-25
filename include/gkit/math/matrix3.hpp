#pragma once

#include "gkit/math/vector3.hpp"

#include <tuple>
#include <optional>


namespace gkit::math {

    class Matrix3 {
    public:
        // Column-major storage (OpenGL-style): m[col][row]
        // Vectors are treated as column vectors: v' = M * v
        // Matrix layout:
        // | m[0][0] m[1][0] m[2][0] |
        // | m[0][1] m[1][1] m[2][1] |
        // | m[0][2] m[1][2] m[2][2] |
        float m[3][3] = { {0.0f, 0.0f, 0.0f},
                          {0.0f, 0.0f, 0.0f},
                          {0.0f, 0.0f, 0.0f} };

    public: // Constructors
        // Default constructor (zero-initialized via in-class member initializer)
        Matrix3() noexcept = default;

        // Create identity matrix (1.0 on diagonal)
        static inline auto identity() noexcept -> Matrix3 {
            Matrix3 result;
            result.m[0][0] = 1.0f; result.m[1][1] = 1.0f; result.m[2][2] = 1.0f;
            return result;
        }

        // Create zero matrix (all elements zero)
        static inline auto zero() noexcept -> Matrix3 {
            return {};
        }

        // Create diagonal matrix with uniform value
        static inline auto from_diagonal(float v) noexcept -> Matrix3 {
            Matrix3 result;
            result.m[0][0] = v;
            result.m[1][1] = v;
            result.m[2][2] = v;
            return result;
        }

        // Create diagonal matrix from vector (diagonal = v.x, v.y, v.z)
        static inline auto from_diagonal(const Vector3& v) noexcept -> Matrix3 {
            Matrix3 result;
            result.m[0][0] = v.x;
            result.m[1][1] = v.y;
            result.m[2][2] = v.z;
            return result;
        }

    public: // Matrix multiplication
        // Matrix * Matrix (column-major): result = this * other
        auto operator*(const Matrix3& other) const noexcept -> Matrix3;

        // Matrix * Vector3 (column vector): v' = M * v
        auto operator*(const Vector3& v) const noexcept -> Vector3;

    public: // Matrix operations
        // Transpose: result[col][row] = mat[row][col]
        static auto transpose(const Matrix3& mat) noexcept -> Matrix3;

        // Determinant of the matrix
        static auto determinant(const Matrix3& mat) noexcept -> float;

        // Inverse matrix. Returns std::nullopt if singular (non-invertible)
        static auto inverse(const Matrix3& mat) noexcept -> std::optional<Matrix3>;

    public: // Properties
        // Returns all elements as a tuple (column-major order)
        [[nodiscard]] inline auto properties() const noexcept -> auto {
            return std::tie(m[0][0], m[1][0], m[2][0],
                           m[0][1], m[1][1], m[2][1],
                           m[0][2], m[1][2], m[2][2]);
        }

    public: // Static factory methods
        // Create rotation matrix from Euler angles (radians)
        // Rotation order: R = Rz(roll) * Ry(yaw) * Rx(pitch)
        static auto from_euler(float pitch, float yaw, float roll) noexcept -> Matrix3;

        // Rotation matrix around X-axis
        static auto rotation_x(float angle) noexcept -> Matrix3;

        // Rotation matrix around Y-axis
        static auto rotation_y(float angle) noexcept -> Matrix3;

        // Rotation matrix around Z-axis
        static auto rotation_z(float angle) noexcept -> Matrix3;

        // Uniform scaling matrix
        static auto scaling(float scale) noexcept -> Matrix3;

        // Non-uniform scaling matrix
        static auto scaling(float sx, float sy, float sz) noexcept -> Matrix3;
    }; // class Matrix3

} // namespace gkit::math
