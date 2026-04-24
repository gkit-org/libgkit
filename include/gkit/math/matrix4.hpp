#pragma once

#include "gkit/math/vector3.hpp"
#include "gkit/math/vector4.hpp"
#include "gkit/math/matrix3.hpp"

#include <cmath>
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

        Matrix4() noexcept = default;

        explicit Matrix4(float diag) noexcept;
        explicit Matrix4(const Matrix3& mat3) noexcept;
        explicit Matrix4(const float* data) noexcept;

    public: // Static factory methods
        static inline auto identity() noexcept -> Matrix4 {
            Matrix4 result;
            result.m[0][0] = 1.0f; result.m[1][1] = 1.0f;
            result.m[2][2] = 1.0f; result.m[3][3] = 1.0f;
            return result;
        }

        static inline auto zero() noexcept -> Matrix4 {
            return {};
        }

    public: // Factory methods - Transformation
        // Translation matrix
        static inline auto translate(const Vector3& translation) noexcept -> Matrix4 {
            Matrix4 result = identity();
            result.m[3][0] = translation.x;
            result.m[3][1] = translation.y;
            result.m[3][2] = translation.z;
            return result;
        }

        // Scale matrix
        static inline auto scale(const Vector3& scale) noexcept -> Matrix4 {
            Matrix4 result = identity();
            result.m[0][0] = scale.x;
            result.m[1][1] = scale.y;
            result.m[2][2] = scale.z;
            return result;
        }

        // Rotation matrix around arbitrary axis (angle in radians)
        static inline auto rotate(float angle, const Vector3& axis) noexcept -> Matrix4 {
            float c = std::cos(angle), s = std::sin(angle);
            float t = 1.0f - c;
            float x = axis.x, y = axis.y, z = axis.z;

            Matrix4 result;
            result.m[0][0] = t * x * x + c;     result.m[1][0] = t * x * y - s * z; result.m[2][0] = t * x * z + s * y;
            result.m[0][1] = t * x * y + s * z; result.m[1][1] = t * y * y + c;     result.m[2][1] = t * y * z - s * x;
            result.m[0][2] = t * x * z - s * y; result.m[1][2] = t * y * z + s * x; result.m[2][2] = t * z * z + c;
            result.m[3][3] = 1.0f;
            return result;
        }

        // Rotation around X-axis (angle in radians)
        static inline auto rotate_x(float angle) noexcept -> Matrix4 {
            float c = std::cos(angle), s = std::sin(angle);
            Matrix4 result = identity();
            result.m[1][1] = c;    result.m[2][1] = s;
            result.m[1][2] = -s;   result.m[2][2] = c;
            return result;
        }

        // Rotation around Y-axis (angle in radians)
        static inline auto rotate_y(float angle) noexcept -> Matrix4 {
            float c = std::cos(angle), s = std::sin(angle);
            Matrix4 result = identity();
            result.m[0][0] = c;    result.m[2][0] = -s;
            result.m[0][2] = s;    result.m[2][2] = c;
            return result;
        }

        // Rotation around Z-axis (angle in radians)
        static inline auto rotate_z(float angle) noexcept -> Matrix4 {
            float c = std::cos(angle), s = std::sin(angle);
            Matrix4 result = identity();
            result.m[0][0] = c;    result.m[1][0] = s;
            result.m[0][1] = -s;   result.m[1][1] = c;
            return result;
        }

    public: // Factory methods - Projection
        // Perspective projection matrix (angle in radians)
        static inline auto perspective(float fov, float aspect, float near, float far) noexcept -> Matrix4 {
            float tan_half_fov = 1.0f / std::tan(fov * 0.5f);

            Matrix4 result = zero();
            result.m[0][0] = 1.0f / (aspect * tan_half_fov);
            result.m[1][1] = 1.0f / tan_half_fov;
            result.m[2][2] = -(far + near) / (far - near);
            result.m[2][3] = -1.0f;
            result.m[3][2] = -(2.0f * far * near) / (far - near);
            return result;
        }

        // Orthographic projection matrix
        static inline auto orthographic(float left, float right, float bottom, float top, float near, float far) noexcept -> Matrix4 {
            float rl = right - left;
            float tb = top - bottom;
            float fn = far - near;

            Matrix4 result = zero();
            result.m[0][0] = 2.0f / rl;
            result.m[1][1] = 2.0f / tb;
            result.m[2][2] = -2.0f / fn;
            result.m[3][0] = -(right + left) / rl;
            result.m[3][1] = -(top + bottom) / tb;
            result.m[3][2] = -(far + near) / fn;
            result.m[3][3] = 1.0f;
            return result;
        }

        // Look-at view matrix
        static inline auto look_at(const Vector3& eye, const Vector3& target, const Vector3& up) noexcept -> Matrix4 {
            Vector3 f = (target - eye).normalize();
            Vector3 s = Vector3::cross(f, up).normalize();
            Vector3 u = Vector3::cross(s, f);

            Matrix4 result = identity();
            result.m[0][0] = s.x;  result.m[1][0] = s.y;  result.m[2][0] = s.z;
            result.m[0][1] = u.x;  result.m[1][1] = u.y;  result.m[2][1] = u.z;
            result.m[0][2] = -f.x; result.m[1][2] = -f.y; result.m[2][2] = -f.z;
            result.m[3][0] = -Vector3::dot(s, eye);
            result.m[3][1] = -Vector3::dot(u, eye);
            result.m[3][2] = Vector3::dot(f, eye);
            return result;
        }

    public: // Factory methods - Static construction
        // Create from rows
        static inline auto from_rows(const Vector4& row0, const Vector4& row1, const Vector4& row2, const Vector4& row3) noexcept -> Matrix4 {
            Matrix4 result;
            result.m[0][0] = row0.x; result.m[1][0] = row0.y; result.m[2][0] = row0.z; result.m[3][0] = row0.w;
            result.m[0][1] = row1.x; result.m[1][1] = row1.y; result.m[2][1] = row1.z; result.m[3][1] = row1.w;
            result.m[0][2] = row2.x; result.m[1][2] = row2.y; result.m[2][2] = row2.z; result.m[3][2] = row2.w;
            result.m[0][3] = row3.x; result.m[1][3] = row3.y; result.m[2][3] = row3.z; result.m[3][3] = row3.w;
            return result;
        }

        // Create from columns
        static inline auto from_columns(const Vector4& col0, const Vector4& col1, const Vector4& col2, const Vector4& col3) noexcept -> Matrix4 {
            Matrix4 result;
            result.m[0][0] = col0.x; result.m[0][1] = col0.y; result.m[0][2] = col0.z; result.m[0][3] = col0.w;
            result.m[1][0] = col1.x; result.m[1][1] = col1.y; result.m[1][2] = col1.z; result.m[1][3] = col1.w;
            result.m[2][0] = col2.x; result.m[2][1] = col2.y; result.m[2][2] = col2.z; result.m[2][3] = col2.w;
            result.m[3][0] = col3.x; result.m[3][1] = col3.y; result.m[3][2] = col3.z; result.m[3][3] = col3.w;
            return result;
        }

        // Rotation between two vectors
        static inline auto rotation_between_vectors(const Vector3& from, const Vector3& to) noexcept -> Matrix4 {
            Vector3 f = from.normalize();
            Vector3 t = to.normalize();
            float cos_theta = Vector3::dot(f, t);

            if (cos_theta >= 1.0f - NORMALIZE_TOLERANCE_32) {
                return identity();
            }
            if (cos_theta <= -1.0f + NORMALIZE_TOLERANCE_32) {
                Vector3 axis = Vector3::cross({1.0f, 0.0f, 0.0f}, f).normalize();
                if (axis.length_sq() < NORMALIZE_TOLERANCE_32) {
                    axis = Vector3::cross({0.0f, 1.0f, 0.0f}, f).normalize();
                }
                return rotate(PI_32, axis);
            }

            Vector3 axis = Vector3::cross(f, t).normalize();
            float angle = std::acos(cos_theta);
            return rotate(angle, axis);
        }

        // Reflection matrix (plane: ax + by + cz + d = 0, stored as Vec4(a,b,c,d))
        static inline auto reflect(const Vector4& plane) noexcept -> Matrix4 {
            float a = plane.x, b = plane.y, c = plane.z, d = plane.w;

            Matrix4 result = identity();
            result.m[0][0] = 1.0f - 2.0f * a * a;
            result.m[0][1] = -2.0f * a * b;
            result.m[0][2] = -2.0f * a * c;
            result.m[0][3] = -2.0f * a * d;

            result.m[1][0] = -2.0f * a * b;
            result.m[1][1] = 1.0f - 2.0f * b * b;
            result.m[1][2] = -2.0f * b * c;
            result.m[1][3] = -2.0f * b * d;

            result.m[2][0] = -2.0f * a * c;
            result.m[2][1] = -2.0f * b * c;
            result.m[2][2] = 1.0f - 2.0f * c * c;
            result.m[2][3] = -2.0f * c * d;

            return result;
        }

        // Shear transformation
        static inline auto shear(const Vector3& shear_factors) noexcept -> Matrix4 {
            Matrix4 result = identity();
            result.m[1][0] = shear_factors.x;
            result.m[2][0] = shear_factors.y;
            result.m[3][0] = shear_factors.z;
            return result;
        }

    public: // Factory methods - Composition
        // Compose TRS matrix (scale -> rotate -> translate)
        static inline auto compose(const Vector3& translation, const Matrix3& rotation, const Vector3& scale) noexcept -> Matrix4 {
            Matrix4 result = identity();
            result.m[0][0] = rotation.m[0][0] * scale.x;
            result.m[0][1] = rotation.m[0][1] * scale.x;
            result.m[0][2] = rotation.m[0][2] * scale.x;

            result.m[1][0] = rotation.m[1][0] * scale.y;
            result.m[1][1] = rotation.m[1][1] * scale.y;
            result.m[1][2] = rotation.m[1][2] * scale.y;

            result.m[2][0] = rotation.m[2][0] * scale.z;
            result.m[2][1] = rotation.m[2][1] * scale.z;
            result.m[2][2] = rotation.m[2][2] * scale.z;

            result.m[3][0] = translation.x;
            result.m[3][1] = translation.y;
            result.m[3][2] = translation.z;
            result.m[3][3] = 1.0f;
            return result;
        }

        // Compose TR matrix (rotate -> translate)
        static inline auto compose(const Vector3& translation, const Matrix3& rotation) noexcept -> Matrix4 {
            Matrix4 result = identity();
            for (int col = 0; col < 3; ++col) {
                for (int row = 0; row < 3; ++row) {
                    result.m[col][row] = rotation.m[col][row];
                }
            }
            result.m[3][0] = translation.x;
            result.m[3][1] = translation.y;
            result.m[3][2] = translation.z;
            result.m[3][3] = 1.0f;
            return result;
        }

    public: // Accessors
        // Access column i
        inline auto operator[](int i) noexcept -> float* { return m[i]; }
        inline auto operator[](int i) const noexcept -> const float* { return m[i]; }

        // Get row i
        [[nodiscard]] inline auto row(int i) const noexcept -> Vector4 {
            return {m[0][i], m[1][i], m[2][i], m[3][i]};
        }

        // Get column j
        [[nodiscard]] inline auto column(int j) const noexcept -> Vector4 {
            return {m[j][0], m[j][1], m[j][2], m[j][3]};
        }

        // Get raw data pointer (column-major)
        inline auto data() noexcept -> float* { return &m[0][0]; }
        [[nodiscard]] inline auto data() const noexcept -> const float* { return &m[0][0]; }

    public: // Mutators
        inline auto set_identity() noexcept -> Matrix4& {
            *this = identity();
            return *this;
        }

        inline auto set_zero() noexcept -> Matrix4& {
            *this = zero();
            return *this;
        }

    public: // Matrix multiplication
        auto operator*(const Matrix4& other) const noexcept -> Matrix4;

        // Matrix * Vec4
        inline auto operator*(const Vector4& v) const noexcept -> Vector4 {
            return {
                m[0][0] * v.x + m[1][0] * v.y + m[2][0] * v.z + m[3][0] * v.w,
                m[0][1] * v.x + m[1][1] * v.y + m[2][1] * v.z + m[3][1] * v.w,
                m[0][2] * v.x + m[1][2] * v.y + m[2][2] * v.z + m[3][2] * v.w,
                m[0][3] * v.x + m[1][3] * v.y + m[2][3] * v.z + m[3][3] * v.w
            };
        }

        // Matrix * Vec3 (assumes w = 1)
        inline auto operator*(const Vector3& v) const noexcept -> Vector3 {
            float w = m[0][3] * v.x + m[1][3] * v.y + m[2][3] * v.z + m[3][3];
            return {
                (m[0][0] * v.x + m[1][0] * v.y + m[2][0] * v.z + m[3][0]) / w,
                (m[0][1] * v.x + m[1][1] * v.y + m[2][1] * v.z + m[3][1]) / w,
                (m[0][2] * v.x + m[1][2] * v.y + m[2][2] * v.z + m[3][2]) / w
            };
        }

        inline auto operator+=(const Matrix4& other) noexcept -> Matrix4& {
            for (int col = 0; col < 4; ++col) {
                for (int row = 0; row < 4; ++row) {
                    m[col][row] += other.m[col][row];
                }
            }
            return *this;
        }

        inline auto operator-=(const Matrix4& other) noexcept -> Matrix4& {
            for (int col = 0; col < 4; ++col) {
                for (int row = 0; row < 4; ++row) {
                    m[col][row] -= other.m[col][row];
                }
            }
            return *this;
        }

        inline auto operator*=(const Matrix4& other) noexcept -> Matrix4& {
            *this = *this * other;
            return *this;
        }

        inline auto operator*=(float scalar) noexcept -> Matrix4& {
            for (int col = 0; col < 4; ++col) {
                for (int row = 0; row < 4; ++row) {
                    m[col][row] *= scalar;
                }
            }
            return *this;
        }

    public: // Comparison operators
        inline auto operator==(const Matrix4& other) const noexcept -> bool {
            for (int col = 0; col < 4; ++col) {
                for (int row = 0; row < 4; ++row) {
                    if (std::abs(m[col][row] - other.m[col][row]) > NORMALIZE_TOLERANCE_32) {
                        return false;
                    }
                }
            }
            return true;
        }

        inline auto operator!=(const Matrix4& other) const noexcept -> bool {
            return !(*this == other);
        }

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

        // Determinant (using cofactor expansion along first row)
        static inline auto determinant(const Matrix4& mat) noexcept -> float {
            float a = mat.m[0][0], b = mat.m[1][0], c = mat.m[2][0], d = mat.m[3][0];
            float e = mat.m[0][1], f = mat.m[1][1], g = mat.m[2][1], h = mat.m[3][1];
            float i = mat.m[0][2], j = mat.m[1][2], k = mat.m[2][2], l = mat.m[3][2];
            float m_ = mat.m[0][3], n = mat.m[1][3], o = mat.m[2][3], p = mat.m[3][3];

            // Cofactor expansion along first row: a*C00 - b*C01 + c*C02 - d*C03
            float C00 = f * (k * p - l * o) - g * (j * p - l * n) + h * (j * o - k * n);
            float C01 = e * (k * p - l * o) - g * (i * p - l * m_) + h * (i * o - k * m_);
            float C02 = e * (j * p - l * n) - f * (i * p - l * m_) + h * (i * n - j * m_);
            float C03 = e * (j * o - k * n) - f * (i * o - k * m_) + g * (i * n - j * m_);

            return a * C00 - b * C01 + c * C02 - d * C03;
        }

        // WARNING: Returns std::nullopt for singular (non-invertible) matrices.
        static auto inverse(const Matrix4& mat) noexcept -> std::optional<Matrix4>;

        // Trace (sum of diagonal elements)
        inline auto trace() const noexcept -> float {
            return m[0][0] + m[1][1] + m[2][2] + m[3][3];
        }

        // Minor (determinant of 3x3 submatrix removing row and col)
        static inline auto minor(const Matrix4& mat, int row, int col) noexcept -> float {
            float sub[3][3];
            int si = 0, sj = 0;
            for (int i = 0; i < 4; ++i) {
                if (i == row) continue;
                sj = 0;
                for (int j = 0; j < 4; ++j) {
                    if (j == col) continue;
                    sub[si][sj] = mat.m[j][i];
                    ++sj;
                }
                ++si;
            }
            return sub[0][0] * (sub[1][1] * sub[2][2] - sub[1][2] * sub[2][1])
                 - sub[0][1] * (sub[1][0] * sub[2][2] - sub[1][2] * sub[2][0])
                 + sub[0][2] * (sub[1][0] * sub[2][1] - sub[1][1] * sub[2][0]);
        }

        // Cofactor
        static inline auto cofactor(const Matrix4& mat, int row, int col) noexcept -> float {
            float m = minor(mat, row, col);
            return ((row + col) % 2 == 0) ? m : -m;
        }

        // Adjugate matrix
        static inline auto adjugate(const Matrix4& mat) noexcept -> Matrix4 {
            Matrix4 result;
            for (int row = 0; row < 4; ++row) {
                for (int col = 0; col < 4; ++col) {
                    result.m[col][row] = cofactor(mat, row, col);
                }
            }
            return result;
        }

    public: // Transformation operations
        // Transform point (applies translation)
        static inline auto transform_point(const Matrix4& m, const Vector3& point) noexcept -> Vector3 {
            Vector4 p{point.x, point.y, point.z, 1.0f};
            Vector4 result = m * p;
            return {result.x, result.y, result.z};
        }

        // Transform vector (no translation)
        static inline auto transform_vector(const Matrix4& m, const Vector3& vector) noexcept -> Vector3 {
            Vector4 v{vector.x, vector.y, vector.z, 0.0f};
            Vector4 result = m * v;
            return {result.x, result.y, result.z};
        }

        static auto transform_plane(const Matrix4& m, const Vector4& plane) noexcept -> Vector4;

        static auto transform_normal(const Matrix4& m, const Vector3& normal) noexcept -> Vector3;

        // Inverse transform point
        static inline auto inverse_transform_point(const Matrix4& m, const Vector3& point) noexcept -> Vector3 {
            auto inv = inverse(m);
            return inv ? transform_point(*inv, point) : point;
        }

        // Inverse transform vector
        static inline auto inverse_transform_vector(const Matrix4& m, const Vector3& vector) noexcept -> Vector3 {
            auto inv = inverse(m);
            return inv ? transform_vector(*inv, vector) : vector;
        }

    public: // Decomposition
        // Extract translation
        static inline auto get_translation(const Matrix4& mat) noexcept -> Vector3 {
            return {mat.m[3][0], mat.m[3][1], mat.m[3][2]};
        }

        // Extract rotation (upper-left 3x3)
        static inline auto get_rotation(const Matrix4& mat) noexcept -> Matrix3 {
            Matrix3 result;
            for (int col = 0; col < 3; ++col) {
                for (int row = 0; row < 3; ++row) {
                    result.m[col][row] = mat.m[col][row];
                }
            }
            return result;
        }

        // Extract scale
        static inline auto get_scale(const Matrix4& mat) noexcept -> Vector3 {
            return {
                Vector3{mat.m[0][0], mat.m[0][1], mat.m[0][2]}.length(),
                Vector3{mat.m[1][0], mat.m[1][1], mat.m[1][2]}.length(),
                Vector3{mat.m[2][0], mat.m[2][1], mat.m[2][2]}.length()
            };
        }

        static auto decompose(const Matrix4& mat) noexcept -> std::tuple<Vector3, Matrix3, Vector3>;

    public: // Property checks
        // Check if affine (last row is [0,0,0,1])
        [[nodiscard]] inline auto is_affine() const noexcept -> bool {
            return std::abs(m[0][3]) < NORMALIZE_TOLERANCE_32
                && std::abs(m[1][3]) < NORMALIZE_TOLERANCE_32
                && std::abs(m[2][3]) < NORMALIZE_TOLERANCE_32
                && std::abs(m[3][3] - 1.0f) < NORMALIZE_TOLERANCE_32;
        }

        // Check if rotation part is orthogonal
        [[nodiscard]] inline auto is_orthogonal() const noexcept -> bool {
            Matrix3 r = get_rotation(*this);
            Matrix3 identity = Matrix3::identity();
            Matrix3 result = r * Matrix3::transpose(r);
            for (int col = 0; col < 3; ++col) {
                for (int row = 0; row < 3; ++row) {
                    if (std::abs(result.m[col][row] - identity.m[col][row]) > NORMALIZE_TOLERANCE_32) {
                        return false;
                    }
                }
            }
            return true;
        }

        // Check if rotation part is orthonormal
        [[nodiscard]] inline auto is_orthonormal() const noexcept -> bool {
            Matrix3 r = get_rotation(*this);
            for (auto & col : r.m) {
                Vector3 c{col[0], col[1], col[2]};
                if (std::abs(c.length_sq() - 1.0f) > NORMALIZE_TOLERANCE_32) {
                    return false;
                }
            }
            return is_orthogonal();
        }

        // Normalize rotation part
        inline auto normalize_rotation() noexcept -> Matrix4& {
            for (int col = 0; col < 3; ++col) {
                Vector3 c{m[col][0], m[col][1], m[col][2]};
                c = c.normalize();
                m[col][0] = c.x;
                m[col][1] = c.y;
                m[col][2] = c.z;
            }
            return *this;
        }

    public: // Interpolation
        // Linear interpolation between matrices
        static inline auto lerp(const Matrix4& a, const Matrix4& b, float t) noexcept -> Matrix4 {
            Matrix4 result;
            for (int col = 0; col < 4; ++col) {
                for (int row = 0; row < 4; ++row) {
                    result.m[col][row] = a.m[col][row] + t * (b.m[col][row] - a.m[col][row]);
                }
            }
            return result;
        }

        // Spherical linear interpolation (requires decompose to TRS first)
        static auto slerp(const Matrix4& a, const Matrix4& b, float t) noexcept -> Matrix4;

    public: // Quaternion helpers (placeholder - quaternion class not implemented yet)
        // Get quaternion from rotation matrix
        static inline auto get_quaternion(const Matrix4& mat) noexcept -> Vector4 {
            Matrix3 r = get_rotation(mat);
            Vector4 q;

            float trace = r.m[0][0] + r.m[1][1] + r.m[2][2];
            if (trace > 0.0f) {
                float s = std::sqrt(trace + 1.0f) * 2.0f;
                q.w = 0.25f * s;
                q.x = (r.m[2][1] - r.m[1][2]) / s;
                q.y = (r.m[0][2] - r.m[2][0]) / s;
                q.z = (r.m[1][0] - r.m[0][1]) / s;
            } else if (r.m[0][0] > r.m[1][1] && r.m[0][0] > r.m[2][2]) {
                float s = std::sqrt(1.0f + r.m[0][0] - r.m[1][1] - r.m[2][2]) * 2.0f;
                q.w = (r.m[2][1] - r.m[1][2]) / s;
                q.x = 0.25f * s;
                q.y = (r.m[0][1] + r.m[1][0]) / s;
                q.z = (r.m[0][2] + r.m[2][0]) / s;
            } else if (r.m[1][1] > r.m[2][2]) {
                float s = std::sqrt(1.0f + r.m[1][1] - r.m[0][0] - r.m[2][2]) * 2.0f;
                q.w = (r.m[0][2] - r.m[2][0]) / s;
                q.x = (r.m[0][1] + r.m[1][0]) / s;
                q.y = 0.25f * s;
                q.z = (r.m[1][2] + r.m[2][1]) / s;
            } else {
                float s = std::sqrt(1.0f + r.m[2][2] - r.m[0][0] - r.m[1][1]) * 2.0f;
                q.w = (r.m[1][0] - r.m[0][1]) / s;
                q.x = (r.m[0][2] + r.m[2][0]) / s;
                q.y = (r.m[1][2] + r.m[2][1]) / s;
                q.z = 0.25f * s;
            }

            return q.normalize();
        }

        // Set rotation from quaternion
        static inline auto set_quaternion(Matrix4& mat, const Vector4& quat) noexcept -> Matrix4 {
            Vector4 q = quat.normalize();
            float xx = q.x * q.x, yy = q.y * q.y, zz = q.z * q.z;
            float xy = q.x * q.y, xz = q.x * q.z, yz = q.y * q.z;
            float wx = q.w * q.x, wy = q.w * q.y, wz = q.w * q.z;

            Matrix3 rot;
            rot.m[0][0] = 1.0f - 2.0f * (yy + zz);
            rot.m[1][0] = 2.0f * (xy + wz);
            rot.m[2][0] = 2.0f * (xz - wy);
            rot.m[0][1] = 2.0f * (xy - wz);
            rot.m[1][1] = 1.0f - 2.0f * (xx + zz);
            rot.m[2][1] = 2.0f * (yz + wx);
            rot.m[0][2] = 2.0f * (xz + wy);
            rot.m[1][2] = 2.0f * (yz - wx);
            rot.m[2][2] = 1.0f - 2.0f * (xx + yy);

            Matrix4 result = mat;
            for (int col = 0; col < 3; ++col) {
                for (int row = 0; row < 3; ++row) {
                    result.m[col][row] = rot.m[col][row];
                }
            }
            return result;
        }

    public: // Output
        [[nodiscard]] auto to_string() const noexcept -> std::string;
        auto print() const noexcept -> void;

    }; // class Matrix4

} // namespace gkit::math
