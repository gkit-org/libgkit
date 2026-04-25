#pragma once

#include "gkit/math/vector3.hpp"
#include "gkit/math/vector4.hpp"
#include "gkit/math/matrix3.hpp"

#include <tuple>
#include <optional>
#include <string>


namespace gkit::math {

    /// @brief 4x4 matrix (column-major, OpenGL-style)
    /// @details Vectors are treated as column vectors: v' = M * v
    /// Matrix layout:
    /// | m[0][0] m[1][0] m[2][0] m[3][0] |
    /// | m[0][1] m[1][1] m[2][1] m[3][1] |
    /// | m[0][2] m[1][2] m[2][2] m[3][2] |
    /// | m[0][3] m[1][3] m[2][3] m[3][3] |
    class Matrix4 {
    public:
        float m[4][4] = { {0.0f, 0.0f, 0.0f, 0.0f},
                          {0.0f, 0.0f, 0.0f, 0.0f},
                          {0.0f, 0.0f, 0.0f, 0.0f},
                          {0.0f, 0.0f, 0.0f, 0.0f} };

    public: // Constructors
        /// @brief Default constructor (zero-initialized)
        Matrix4() noexcept = default;

        /// @brief Create diagonal matrix (value on diagonal, zeros elsewhere)
        /// @param diag Value for diagonal elements
        explicit Matrix4(float diag) noexcept;

        /// @brief Create 4x4 matrix from 3x3 rotation (sets last row/column to identity)
        /// @param mat3 3x3 rotation matrix
        explicit Matrix4(const Matrix3& mat3) noexcept;

        /// @brief Create matrix from raw float array (column-major, 16 elements)
        /// @param data Pointer to 16 floats in column-major order
        explicit Matrix4(const float* data) noexcept;

    public: // Static factory methods
        /// @brief Create identity matrix (1.0 on diagonal)
        static inline auto identity() noexcept -> Matrix4 {
            Matrix4 result;
            result.m[0][0] = 1.0f; result.m[1][1] = 1.0f;
            result.m[2][2] = 1.0f; result.m[3][3] = 1.0f;
            return result;
        }

        /// @brief Create zero matrix (all elements zero)
        static inline auto zero() noexcept -> Matrix4 {
            return {};
        }

    public: // Factory methods - Transformation
        /// @brief Translation matrix: moves points by the given translation vector
        /// @param translation Translation vector (x, y, z)
        static auto translate(const Vector3& translation) noexcept -> Matrix4;

        /// @brief Scale matrix: scales points by the given scale vector
        /// @param scale Scale vector (sx, sy, sz)
        static auto scale(const Vector3& scale) noexcept -> Matrix4;

        /// @brief Rotation matrix around arbitrary axis
        /// @param angle Rotation angle in radians
        /// @param axis Rotation axis (will be normalized)
        static auto rotate(float angle, const Vector3& axis) noexcept -> Matrix4;

        /// @brief Rotation matrix around X-axis
        /// @param angle Rotation angle in radians
        static auto rotate_x(float angle) noexcept -> Matrix4;

        /// @brief Rotation matrix around Y-axis
        /// @param angle Rotation angle in radians
        static auto rotate_y(float angle) noexcept -> Matrix4;

        /// @brief Rotation matrix around Z-axis
        /// @param angle Rotation angle in radians
        static auto rotate_z(float angle) noexcept -> Matrix4;

    public: // Factory methods - Projection
        /// @brief Perspective projection matrix (OpenGL convention, far > near)
        /// @param fov Vertical field of view in radians
        /// @param aspect Aspect ratio (width / height)
        /// @param near Near clipping plane distance (must be > 0)
        /// @param far Far clipping plane distance (must be > near)
        static auto perspective(float fov, float aspect, float near, float far) noexcept -> Matrix4;

        /// @brief Orthographic projection matrix
        /// @param left Left clipping plane
        /// @param right Right clipping plane
        /// @param bottom Bottom clipping plane
        /// @param top Top clipping plane
        /// @param near Near clipping plane
        /// @param far Far clipping plane
        static auto orthographic(float left, float right, float bottom, float top, float near, float far) noexcept -> Matrix4;

        /// @brief Look-at view matrix (camera at eye, looking at target)
        /// @param eye Camera position
        /// @param target Look-at target position
        /// @param up Up vector (used to define camera orientation)
        static auto look_at(const Vector3& eye, const Vector3& target, const Vector3& up) noexcept -> Matrix4;

    public: // Factory methods - Static construction
        /// @brief Create matrix from row vectors
        /// @param row0 First row (x, y, z, w)
        /// @param row1 Second row
        /// @param row2 Third row
        /// @param row3 Fourth row
        static auto from_rows(const Vector4& row0, const Vector4& row1, const Vector4& row2, const Vector4& row3) noexcept -> Matrix4;

        /// @brief Create matrix from column vectors
        /// @param col0 First column (x, y, z, w)
        /// @param col1 Second column
        /// @param col2 Third column
        /// @param col3 Fourth column
        static auto from_columns(const Vector4& col0, const Vector4& col1, const Vector4& col2, const Vector4& col3) noexcept -> Matrix4;

        /// @brief Rotation matrix that rotates 'from' vector to 'to' vector
        /// @param from Source direction vector (will be normalized)
        /// @param to Target direction vector (will be normalized)
        static auto rotation_between_vectors(const Vector3& from, const Vector3& to) noexcept -> Matrix4;

        /// @brief Reflection matrix across a plane
        /// @param plane Plane defined as {nx, ny, nz, d} where n is normal, d is distance from origin
        static auto reflect(const Vector4& plane) noexcept -> Matrix4;

        /// @brief Shear transformation matrix
        /// @param shear_factors Shear factors {x_y, x_z, y_x}
        /// @details Shear mapping: (x, y, z) -> (x + shear_factors.z*y, y + shear_factors.x*x, z + shear_factors.y*x)
        static inline auto shear(const Vector3& shear_factors) noexcept -> Matrix4 {
            Matrix4 result = identity();
            result.m[1][0] = shear_factors.x;
            result.m[2][0] = shear_factors.y;
            result.m[0][1] = shear_factors.z;
            return result;
        }

    public: // Factory methods - Composition
        /// @brief Compose TRS (Translation * Rotation * Scale) matrix
        /// @param translation Translation vector
        /// @param rotation Rotation matrix (3x3)
        /// @param scale Scale vector
        static auto compose(const Vector3& translation, const Matrix3& rotation, const Vector3& scale) noexcept -> Matrix4;

        /// @brief Compose TR (Translation * Rotation) matrix
        /// @param translation Translation vector
        /// @param rotation Rotation matrix (3x3)
        static auto compose(const Vector3& translation, const Matrix3& rotation) noexcept -> Matrix4;

    public: // Accessors
        /// @brief Access column i (returns pointer to column data)
        /// @param i Column index (0-3)
        inline auto operator[](int i) noexcept -> float* { return m[i]; }
        inline auto operator[](int i) const noexcept -> const float* { return m[i]; }

        /// @brief Get row i as Vector4
        /// @param i Row index (0-3)
        [[nodiscard]] inline auto row(int i) const noexcept -> Vector4 {
            return {m[0][i], m[1][i], m[2][i], m[3][i]};
        }

        /// @brief Get column j as Vector4
        /// @param j Column index (0-3)
        [[nodiscard]] inline auto column(int j) const noexcept -> Vector4 {
            return {m[j][0], m[j][1], m[j][2], m[j][3]};
        }

        /// @brief Get raw data pointer (column-major, 16 floats)
        inline auto data() noexcept -> float* { return &m[0][0]; }
        [[nodiscard]] inline auto data() const noexcept -> const float* { return &m[0][0]; }

    public: // Mutators
        /// @brief Set this matrix to identity
        inline auto set_identity() noexcept -> Matrix4& {
            *this = identity();
            return *this;
        }

        /// @brief Set this matrix to zero
        inline auto set_zero() noexcept -> Matrix4& {
            *this = zero();
            return *this;
        }

    public: // Matrix multiplication
        /// @brief Matrix * Matrix: result = this * other
        auto operator*(const Matrix4& other) const noexcept -> Matrix4;

        /// @brief Matrix * Vector4 (column vector): v' = M * v
        auto operator*(const Vector4& v) const noexcept -> Vector4;

        /// @brief Matrix * Vector3 (assumes w=1, performs perspective division)
        auto operator*(const Vector3& v) const noexcept -> Vector3;

        /// @brief Compound addition
        auto operator+=(const Matrix4& other) noexcept -> Matrix4&;

        /// @brief Compound subtraction
        auto operator-=(const Matrix4& other) noexcept -> Matrix4&;

        /// @brief Compound matrix multiplication
        auto operator*=(const Matrix4& other) noexcept -> Matrix4&;

        /// @brief Compound scalar multiplication
        auto operator*=(float scalar) noexcept -> Matrix4&;

    public: // Comparison operators
        /// @brief Equality check with floating-point tolerance
        auto operator==(const Matrix4& other) const noexcept -> bool;

        /// @brief Inequality check
        auto operator!=(const Matrix4& other) const noexcept -> bool;

    public: // Matrix operations
        /// @brief Transpose: result[col][row] = mat[row][col]
        static inline auto transpose(const Matrix4& mat) noexcept -> Matrix4 {
            Matrix4 result;
            for (int col = 0; col < 4; ++col) {
                for (int row = 0; row < 4; ++row) {
                    result.m[col][row] = mat.m[row][col];
                }
            }
            return result;
        }

        /// @brief Determinant of the matrix
        static auto determinant(const Matrix4& mat) noexcept -> float;

        /// @brief Inverse matrix. Returns std::nullopt if singular (non-invertible)
        static auto inverse(const Matrix4& mat) noexcept -> std::optional<Matrix4>;

        /// @brief Sum of diagonal elements
        [[nodiscard]] inline auto trace() const noexcept -> float {
            return m[0][0] + m[1][1] + m[2][2] + m[3][3];
        }

        /// @brief Minor: determinant of 3x3 submatrix obtained by removing row and column
        /// @param mat Source matrix
        /// @param row Row to remove (0-3)
        /// @param col Column to remove (0-3)
        static auto minor(const Matrix4& mat, int row, int col) noexcept -> float;

        /// @brief Cofactor: minor with sign adjustment ((row+col) % 2 ? -minor : minor)
        /// @param mat Source matrix
        /// @param row Row index (0-3)
        /// @param col Column index (0-3)
        static auto cofactor(const Matrix4& mat, int row, int col) noexcept -> float;

        /// @brief Adjugate matrix (transpose of cofactor matrix)
        /// @param mat Source matrix
        static auto adjugate(const Matrix4& mat) noexcept -> Matrix4;

    public: // Transformation operations
        /// @brief Transform point (applies translation and rotation/scale, performs perspective division)
        /// @param m Transformation matrix
        /// @param point Point to transform
        static auto transform_point(const Matrix4& m, const Vector3& point) noexcept -> Vector3;

        /// @brief Transform direction vector (ignores translation)
        /// @param m Transformation matrix
        /// @param vector Direction vector to transform
        static auto transform_vector(const Matrix4& m, const Vector3& vector) noexcept -> Vector3;

        /// @brief Transform plane (normal + distance)
        /// @param m Transformation matrix
        /// @param plane Plane to transform (normal.xyz, distance.w)
        static auto transform_plane(const Matrix4& m, const Vector4& plane) noexcept -> Vector4;

        /// @brief Transform normal (for surface normals, uses inverse transpose)
        /// @param m Transformation matrix
        /// @param normal Normal vector to transform
        static auto transform_normal(const Matrix4& m, const Vector3& normal) noexcept -> Vector3;

        /// @brief Inverse transform point
        /// @param m Transformation matrix
        /// @param point Point to transform
        static auto inverse_transform_point(const Matrix4& m, const Vector3& point) noexcept -> Vector3;

        /// @brief Inverse transform vector
        /// @param m Transformation matrix
        /// @param vector Vector to transform
        static auto inverse_transform_vector(const Matrix4& m, const Vector3& vector) noexcept -> Vector3;

    public: // Decomposition
        /// @brief Extract translation vector from last column
        /// @param mat Source matrix
        static inline auto get_translation(const Matrix4& mat) noexcept -> Vector3 {
            return {mat.m[3][0], mat.m[3][1], mat.m[3][2]};
        }

        /// @brief Extract rotation matrix (upper-left 3x3)
        /// @param mat Source matrix
        static inline auto get_rotation(const Matrix4& mat) noexcept -> Matrix3 {
            Matrix3 result;
            for (int col = 0; col < 3; ++col) {
                for (int row = 0; row < 3; ++row) {
                    result.m[col][row] = mat.m[col][row];
                }
            }
            return result;
        }

        /// @brief Extract scale vector (length of each column)
        /// @param mat Source matrix
        static inline auto get_scale(const Matrix4& mat) noexcept -> Vector3 {
            return {
                Vector3{mat.m[0][0], mat.m[0][1], mat.m[0][2]}.length(),
                Vector3{mat.m[1][0], mat.m[1][1], mat.m[1][2]}.length(),
                Vector3{mat.m[2][0], mat.m[2][1], mat.m[2][2]}.length()
            };
        }

        /// @brief Decompose matrix into translation, rotation, and scale
        /// @param mat Source matrix
        static auto decompose(const Matrix4& mat) noexcept -> std::tuple<Vector3, Matrix3, Vector3>;

    public: // Property checks
        /// @brief Check if matrix is affine (last row is [0,0,0,1])
        [[nodiscard]] auto is_affine() const noexcept -> bool;

        /// @brief Check if rotation part is orthogonal (R * R^T = I)
        [[nodiscard]] auto is_orthogonal() const noexcept -> bool;

        /// @brief Check if rotation part is orthonormal (orthogonal + unit columns)
        [[nodiscard]] auto is_orthonormal() const noexcept -> bool;

        /// @brief Normalize rotation columns to unit length
        auto normalize_rotation() noexcept -> Matrix4&;

    public: // Interpolation
        /// @brief Linear interpolation between two matrices
        /// @param a First matrix
        /// @param b Second matrix
        /// @param t Interpolation parameter (0 = a, 1 = b)
        static auto lerp(const Matrix4& a, const Matrix4& b, float t) noexcept -> Matrix4;

        /// @brief Spherical interpolation between two matrices (interpolates rotation)
        /// @param a First matrix
        /// @param b Second matrix
        /// @param t Interpolation parameter (0 = a, 1 = b)
        static auto slerp(const Matrix4& a, const Matrix4& b, float t) noexcept -> Matrix4;

    public: // Quaternion helpers
        /// @brief Extract quaternion from rotation matrix
        /// @param mat Source matrix (must be rotation-only)
        static auto get_quaternion(const Matrix4& mat) noexcept -> Vector4;

        /// @brief Set rotation matrix from quaternion
        /// @param mat Matrix to modify
        /// @param quat Quaternion (will be normalized)
        static auto set_quaternion(Matrix4& mat, const Vector4& quat) noexcept -> Matrix4&;

    public: // Output
        /// @brief Convert to string representation
        [[nodiscard]] auto to_string() const noexcept -> std::string;

        /// @brief Print matrix to stdout
        auto print() const noexcept -> void;

    }; // class Matrix4

} // namespace gkit::math
