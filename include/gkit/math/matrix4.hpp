#pragma once

#include "gkit/math/vector3.hpp"
#include "gkit/math/vector4.hpp"
#include "gkit/math/matrix3.hpp"

#include <tuple>
#include <optional>
#include <string>


namespace gkit::math {

    class Matrix4 {
    public:
        // Column-major storage (OpenGL-style): m[col][row]
        // Vectors are treated as column vectors: v' = M * v
        // Matrix layout:
        // | m[0][0] m[1][0] m[2][0] m[3][0] |
        // | m[0][1] m[1][1] m[2][1] m[3][1] |
        // | m[0][2] m[1][2] m[2][2] m[3][2] |
        // | m[0][3] m[1][3] m[2][3] m[3][3] |
        float m[4][4] = { {0.0f, 0.0f, 0.0f, 0.0f},
                          {0.0f, 0.0f, 0.0f, 0.0f},
                          {0.0f, 0.0f, 0.0f, 0.0f},
                          {0.0f, 0.0f, 0.0f, 0.0f} };

    public: // Constructors
        // Default constructor (zero-initialized)
        Matrix4() noexcept = default;

        // Create diagonal matrix (value on diagonal, zeros elsewhere)
        explicit Matrix4(float diag) noexcept;

        // Create 4x4 matrix from 3x3 rotation (sets last row/column to identity)
        explicit Matrix4(const Matrix3& mat3) noexcept;

        // Create matrix from raw float array (column-major, 16 elements)
        explicit Matrix4(const float* data) noexcept;

    public: // Static factory methods
        // Create identity matrix (1.0 on diagonal)
        static inline auto identity() noexcept -> Matrix4 {
            Matrix4 result;
            result.m[0][0] = 1.0f; result.m[1][1] = 1.0f;
            result.m[2][2] = 1.0f; result.m[3][3] = 1.0f;
            return result;
        }

        // Create zero matrix (all elements zero)
        static inline auto zero() noexcept -> Matrix4 {
            return {};
        }

    public: // Factory methods - Transformation
        // Translation matrix: moves points by the given translation vector
        static auto translate(const Vector3& translation) noexcept -> Matrix4;

        // Scale matrix: scales points by the given scale vector
        static auto scale(const Vector3& scale) noexcept -> Matrix4;

        // Rotation matrix around arbitrary axis (angle in radians)
        static auto rotate(float angle, const Vector3& axis) noexcept -> Matrix4;

        // Rotation matrix around X-axis (angle in radians)
        static auto rotate_x(float angle) noexcept -> Matrix4;

        // Rotation matrix around Y-axis (angle in radians)
        static auto rotate_y(float angle) noexcept -> Matrix4;

        // Rotation matrix around Z-axis (angle in radians)
        static auto rotate_z(float angle) noexcept -> Matrix4;

    public: // Factory methods - Projection
        // Perspective projection matrix (OpenGL convention, far > near)
        static auto perspective(float fov, float aspect, float near, float far) noexcept -> Matrix4;

        // Orthographic projection matrix
        static auto orthographic(float left, float right, float bottom, float top, float near, float far) noexcept -> Matrix4;

        // Look-at view matrix (camera at eye, looking at target)
        static auto look_at(const Vector3& eye, const Vector3& target, const Vector3& up) noexcept -> Matrix4;

    public: // Factory methods - Static construction
        // Create matrix from row vectors
        static auto from_rows(const Vector4& row0, const Vector4& row1, const Vector4& row2, const Vector4& row3) noexcept -> Matrix4;

        // Create matrix from column vectors
        static auto from_columns(const Vector4& col0, const Vector4& col1, const Vector4& col2, const Vector4& col3) noexcept -> Matrix4;

        // Rotation matrix that rotates 'from' vector to 'to' vector
        static auto rotation_between_vectors(const Vector3& from, const Vector3& to) noexcept -> Matrix4;

        // Reflection matrix across a plane (plane = {nx, ny, nz, d} where n is normal, d is distance from origin)
        static auto reflect(const Vector4& plane) noexcept -> Matrix4;

        // Shear transformation matrix using three shear components:
        // shear_factors = {x_y, x_z, y_x}
        // Shear mapping: (x, y, z) -> (x + shear_factors.z*y, y + shear_factors.x*x, z + shear_factors.y*x)
        static inline auto shear(const Vector3& shear_factors) noexcept -> Matrix4 {
            Matrix4 result = identity();
            result.m[1][0] = shear_factors.x;
            result.m[2][0] = shear_factors.y;
            result.m[0][1] = shear_factors.z;
            return result;
        }

    public: // Factory methods - Composition
        // Compose TRS (Translation * Rotation * Scale) matrix
        static auto compose(const Vector3& translation, const Matrix3& rotation, const Vector3& scale) noexcept -> Matrix4;

        // Compose TR (Translation * Rotation) matrix
        static auto compose(const Vector3& translation, const Matrix3& rotation) noexcept -> Matrix4;

    public: // Accessors
        // Access column i (returns pointer to column data)
        inline auto operator[](int i) noexcept -> float* { return m[i]; }
        inline auto operator[](int i) const noexcept -> const float* { return m[i]; }

        // Get row i as Vector4
        [[nodiscard]] inline auto row(int i) const noexcept -> Vector4 {
            return {m[0][i], m[1][i], m[2][i], m[3][i]};
        }

        // Get column j as Vector4
        [[nodiscard]] inline auto column(int j) const noexcept -> Vector4 {
            return {m[j][0], m[j][1], m[j][2], m[j][3]};
        }

        // Get raw data pointer (column-major, 16 floats)
        inline auto data() noexcept -> float* { return &m[0][0]; }
        [[nodiscard]] inline auto data() const noexcept -> const float* { return &m[0][0]; }

    public: // Mutators
        // Set this matrix to identity
        inline auto set_identity() noexcept -> Matrix4& {
            *this = identity();
            return *this;
        }

        // Set this matrix to zero
        inline auto set_zero() noexcept -> Matrix4& {
            *this = zero();
            return *this;
        }

    public: // Matrix multiplication
        // Matrix * Matrix: result = this * other
        auto operator*(const Matrix4& other) const noexcept -> Matrix4;

        // Matrix * Vector4 (column vector): v' = M * v
        auto operator*(const Vector4& v) const noexcept -> Vector4;

        // Matrix * Vector3 (assumes w=1, performs perspective division)
        auto operator*(const Vector3& v) const noexcept -> Vector3;

        auto operator+=(const Matrix4& other) noexcept -> Matrix4&;
        auto operator-=(const Matrix4& other) noexcept -> Matrix4&;
        auto operator*=(const Matrix4& other) noexcept -> Matrix4&;
        auto operator*=(float scalar) noexcept -> Matrix4&;

    public: // Comparison operators
        // Equality check with floating-point tolerance
        auto operator==(const Matrix4& other) const noexcept -> bool;
        auto operator!=(const Matrix4& other) const noexcept -> bool;

    public: // Matrix operations
        // Transpose: result[col][row] = mat[row][col]
        static inline auto transpose(const Matrix4& mat) noexcept -> Matrix4 {
            Matrix4 result;
            for (int col = 0; col < 4; ++col) {
                for (int row = 0; row < 4; ++row) {
                    result.m[col][row] = mat.m[row][col];
                }
            }
            return result;
        }

        // Determinant of the matrix
        static auto determinant(const Matrix4& mat) noexcept -> float;

        // Inverse matrix. Returns std::nullopt if singular (non-invertible)
        static auto inverse(const Matrix4& mat) noexcept -> std::optional<Matrix4>;

        // Sum of diagonal elements
        [[nodiscard]] inline auto trace() const noexcept -> float {
            return m[0][0] + m[1][1] + m[2][2] + m[3][3];
        }

        // Minor: determinant of 3x3 submatrix obtained by removing row and column
        static auto minor(const Matrix4& mat, int row, int col) noexcept -> float;

        // Cofactor: minor with sign adjustment ((row+col) % 2 ? -minor : minor)
        static auto cofactor(const Matrix4& mat, int row, int col) noexcept -> float;

        // Adjugate matrix (transpose of cofactor matrix)
        static auto adjugate(const Matrix4& mat) noexcept -> Matrix4;

    public: // Transformation operations
        // Transform point (applies translation and rotation/scale, performs perspective division)
        static auto transform_point(const Matrix4& m, const Vector3& point) noexcept -> Vector3;

        // Transform direction vector (ignores translation)
        static auto transform_vector(const Matrix4& m, const Vector3& vector) noexcept -> Vector3;

        // Transform plane (normal + distance)
        static auto transform_plane(const Matrix4& m, const Vector4& plane) noexcept -> Vector4;

        // Transform normal (for surface normals, uses inverse transpose)
        static auto transform_normal(const Matrix4& m, const Vector3& normal) noexcept -> Vector3;

        // Inverse transform point
        static auto inverse_transform_point(const Matrix4& m, const Vector3& point) noexcept -> Vector3;

        // Inverse transform vector
        static auto inverse_transform_vector(const Matrix4& m, const Vector3& vector) noexcept -> Vector3;

    public: // Decomposition
        // Extract translation vector from last column
        static inline auto get_translation(const Matrix4& mat) noexcept -> Vector3 {
            return {mat.m[3][0], mat.m[3][1], mat.m[3][2]};
        }

        // Extract rotation matrix (upper-left 3x3)
        static inline auto get_rotation(const Matrix4& mat) noexcept -> Matrix3 {
            Matrix3 result;
            for (int col = 0; col < 3; ++col) {
                for (int row = 0; row < 3; ++row) {
                    result.m[col][row] = mat.m[col][row];
                }
            }
            return result;
        }

        // Extract scale vector (length of each column)
        static inline auto get_scale(const Matrix4& mat) noexcept -> Vector3 {
            return {
                Vector3{mat.m[0][0], mat.m[0][1], mat.m[0][2]}.length(),
                Vector3{mat.m[1][0], mat.m[1][1], mat.m[1][2]}.length(),
                Vector3{mat.m[2][0], mat.m[2][1], mat.m[2][2]}.length()
            };
        }

        // Decompose matrix into translation, rotation, and scale
        static auto decompose(const Matrix4& mat) noexcept -> std::tuple<Vector3, Matrix3, Vector3>;

    public: // Property checks
        // Check if matrix is affine (last row is [0,0,0,1])
        [[nodiscard]] auto is_affine() const noexcept -> bool;

        // Check if rotation part is orthogonal (R * R^T = I)
        [[nodiscard]] auto is_orthogonal() const noexcept -> bool;

        // Check if rotation part is orthonormal (orthogonal + unit columns)
        [[nodiscard]] auto is_orthonormal() const noexcept -> bool;

        // Normalize rotation columns to unit length
        auto normalize_rotation() noexcept -> Matrix4&;

    public: // Interpolation
        // Linear interpolation between two matrices
        static auto lerp(const Matrix4& a, const Matrix4& b, float t) noexcept -> Matrix4;

        // Spherical interpolation between two matrices (interpolates rotation)
        static auto slerp(const Matrix4& a, const Matrix4& b, float t) noexcept -> Matrix4;

    public: // Quaternion helpers
        // Extract quaternion from rotation matrix
        static auto get_quaternion(const Matrix4& mat) noexcept -> Vector4;

        // Set rotation matrix from quaternion
        static auto set_quaternion(Matrix4& mat, const Vector4& quat) noexcept -> Matrix4&;

    public: // Output
        // Convert to string representation
        [[nodiscard]] auto to_string() const noexcept -> std::string;

        // Print matrix to stdout
        auto print() const noexcept -> void;

    }; // class Matrix4

} // namespace gkit::math
