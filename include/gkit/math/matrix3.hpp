#pragma once

#include "gkit/math/vector3.hpp"

#include <tuple>
#include <optional>


namespace gkit::math {

    /// @brief 3x3 matrix (column-major, OpenGL-style)
    /// @details Vectors are treated as column vectors: v' = M * v
    /// Matrix layout:
    /// | m[0][0] m[1][0] m[2][0] |
    /// | m[0][1] m[1][1] m[2][1] |
    /// | m[0][2] m[1][2] m[2][2] |
    class Matrix3 {
    public:
        float m[3][3] = { {0.0f, 0.0f, 0.0f},
                          {0.0f, 0.0f, 0.0f},
                          {0.0f, 0.0f, 0.0f} };

    public: // Constructors
        /// @brief Default constructor (zero-initialized via in-class member initializer)
        Matrix3() noexcept = default;

        /// @brief Create identity matrix (1.0 on diagonal)
        static inline auto identity() noexcept -> Matrix3 {
            Matrix3 result;
            result.m[0][0] = 1.0f; result.m[1][1] = 1.0f; result.m[2][2] = 1.0f;
            return result;
        }

        /// @brief Create zero matrix (all elements zero)
        static inline auto zero() noexcept -> Matrix3 {
            return {};
        }

        /// @brief Create diagonal matrix with uniform value
        static inline auto from_diagonal(float v) noexcept -> Matrix3 {
            Matrix3 result;
            result.m[0][0] = v;
            result.m[1][1] = v;
            result.m[2][2] = v;
            return result;
        }

        /// @brief Create diagonal matrix from vector (diagonal = v.x, v.y, v.z)
        static inline auto from_diagonal(const Vector3& v) noexcept -> Matrix3 {
            Matrix3 result;
            result.m[0][0] = v.x;
            result.m[1][1] = v.y;
            result.m[2][2] = v.z;
            return result;
        }

    public: // Matrix multiplication
        /// @brief Matrix * Matrix (column-major): result = this * other
        auto operator*(const Matrix3& other) const noexcept -> Matrix3;

        /// @brief Matrix * Vector3 (column vector): v' = M * v
        auto operator*(const Vector3& v) const noexcept -> Vector3;

    public: // Matrix operations
        /// @brief Transpose: result[col][row] = mat[row][col]
        static auto transpose(const Matrix3& mat) noexcept -> Matrix3;

        /// @brief Determinant of the matrix
        static auto determinant(const Matrix3& mat) noexcept -> float;

        /// @brief Inverse matrix. Returns std::nullopt if singular (non-invertible)
        static auto inverse(const Matrix3& mat) noexcept -> std::optional<Matrix3>;

    public: // Properties
        /// @brief Returns all elements as a tuple (column-major order)
        [[nodiscard]] inline auto properties() const noexcept -> auto {
            return std::tie(m[0][0], m[1][0], m[2][0],
                           m[0][1], m[1][1], m[2][1],
                           m[0][2], m[1][2], m[2][2]);
        }

    public: // Static factory methods
        /// @brief Create rotation matrix from Euler angles (radians)
        /// @param pitch Rotation around X-axis (radians)
        /// @param yaw Rotation around Y-axis (radians)
        /// @param roll Rotation around Z-axis (radians)
        /// @details Rotation order: R = Rz(roll) * Ry(yaw) * Rx(pitch)
        static auto from_euler(float pitch, float yaw, float roll) noexcept -> Matrix3;

        /// @brief Rotation matrix around X-axis
        /// @param angle Rotation angle in radians
        static auto rotation_x(float angle) noexcept -> Matrix3;

        /// @brief Rotation matrix around Y-axis
        /// @param angle Rotation angle in radians
        static auto rotation_y(float angle) noexcept -> Matrix3;

        /// @brief Rotation matrix around Z-axis
        /// @param angle Rotation angle in radians
        static auto rotation_z(float angle) noexcept -> Matrix3;

        /// @brief Uniform scaling matrix
        /// @param scale Uniform scale factor
        static auto scaling(float scale) noexcept -> Matrix3;

        /// @brief Non-uniform scaling matrix
        /// @param sx Scale factor for X-axis
        /// @param sy Scale factor for Y-axis
        /// @param sz Scale factor for Z-axis
        static auto scaling(float sx, float sy, float sz) noexcept -> Matrix3;
    }; // class Matrix3

} // namespace gkit::math
