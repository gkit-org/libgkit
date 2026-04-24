#include "gkit/math/matrix4.hpp"

#include <iomanip>
#include <sstream>
#include <cstdio>

namespace gkit::math {

    Matrix4::Matrix4(float diag) noexcept {
        m[0][0] = diag; m[1][1] = diag; m[2][2] = diag; m[3][3] = diag;
    }

    Matrix4::Matrix4(const Matrix3& mat3) noexcept {
        for (int col = 0; col < 3; ++col) {
            for (int row = 0; row < 3; ++row) {
                m[col][row] = mat3.m[col][row];
            }
        }
        m[0][3] = 0.0f; m[1][3] = 0.0f; m[2][3] = 0.0f; m[3][3] = 1.0f;
        m[3][0] = 0.0f; m[3][1] = 0.0f; m[3][2] = 0.0f;
    }

    Matrix4::Matrix4(const float* data) noexcept {
        for (int col = 0; col < 4; ++col) {
            for (int row = 0; row < 4; ++row) {
                m[col][row] = data[col * 4 + row];
            }
        }
    }

    auto Matrix4::decompose(const Matrix4& mat) noexcept -> std::tuple<Vector3, Matrix3, Vector3> {
        Vector3 translation = get_translation(mat);
        Vector3 scale = get_scale(mat);

        Matrix3 rotation;
        if (scale.x > NORMALIZE_TOLERANCE_32) {
            rotation.m[0][0] = mat.m[0][0] / scale.x;
            rotation.m[0][1] = mat.m[0][1] / scale.x;
            rotation.m[0][2] = mat.m[0][2] / scale.x;
        }
        if (scale.y > NORMALIZE_TOLERANCE_32) {
            rotation.m[1][0] = mat.m[1][0] / scale.y;
            rotation.m[1][1] = mat.m[1][1] / scale.y;
            rotation.m[1][2] = mat.m[1][2] / scale.y;
        }
        if (scale.z > NORMALIZE_TOLERANCE_32) {
            rotation.m[2][0] = mat.m[2][0] / scale.z;
            rotation.m[2][1] = mat.m[2][1] / scale.z;
            rotation.m[2][2] = mat.m[2][2] / scale.z;
        }

        return {translation, rotation, scale};
    }

    auto Matrix4::operator*(const Matrix4& other) const noexcept -> Matrix4 {
        Matrix4 result;
        for (int col = 0; col < 4; ++col) {
            for (int row = 0; row < 4; ++row) {
                result.m[col][row] = 0.0f;
                for (int k = 0; k < 4; ++k) {
                    result.m[col][row] += this->m[k][row] * other.m[col][k];
                }
            }
        }
        return result;
    }

    auto Matrix4::operator*(const Vector4& v) const noexcept -> Vector4 {
        return {
            m[0][0] * v.x + m[1][0] * v.y + m[2][0] * v.z + m[3][0] * v.w,
            m[0][1] * v.x + m[1][1] * v.y + m[2][1] * v.z + m[3][1] * v.w,
            m[0][2] * v.x + m[1][2] * v.y + m[2][2] * v.z + m[3][2] * v.w,
            m[0][3] * v.x + m[1][3] * v.y + m[2][3] * v.z + m[3][3] * v.w
        };
    }

    auto Matrix4::operator*(const Vector3& v) const noexcept -> Vector3 {
        float w = m[0][3] * v.x + m[1][3] * v.y + m[2][3] * v.z + m[3][3];
        return {
            (m[0][0] * v.x + m[1][0] * v.y + m[2][0] * v.z + m[3][0]) / w,
            (m[0][1] * v.x + m[1][1] * v.y + m[2][1] * v.z + m[3][1]) / w,
            (m[0][2] * v.x + m[1][2] * v.y + m[2][2] * v.z + m[3][2]) / w
        };
    }

    auto Matrix4::operator+=(const Matrix4& other) noexcept -> Matrix4& {
        for (int col = 0; col < 4; ++col) {
            for (int row = 0; row < 4; ++row) {
                m[col][row] += other.m[col][row];
            }
        }
        return *this;
    }

    auto Matrix4::operator-=(const Matrix4& other) noexcept -> Matrix4& {
        for (int col = 0; col < 4; ++col) {
            for (int row = 0; row < 4; ++row) {
                m[col][row] -= other.m[col][row];
            }
        }
        return *this;
    }

    auto Matrix4::operator*=(const Matrix4& other) noexcept -> Matrix4& {
        *this = *this * other;
        return *this;
    }

    auto Matrix4::operator*=(float scalar) noexcept -> Matrix4& {
        for (auto & col : m) {
            for (float & row : col) {
                row *= scalar;
            }
        }
        return *this;
    }

    auto Matrix4::operator==(const Matrix4& other) const noexcept -> bool {
        for (int col = 0; col < 4; ++col) {
            for (int row = 0; row < 4; ++row) {
                if (std::abs(m[col][row] - other.m[col][row]) > NORMALIZE_TOLERANCE_32) {
                    return false;
                }
            }
        }
        return true;
    }

    auto Matrix4::operator!=(const Matrix4& other) const noexcept -> bool {
        return !(*this == other);
    }

    auto Matrix4::slerp(const Matrix4& a, const Matrix4& b, float t) noexcept -> Matrix4 {
        auto [trans_a, rot_a, scale_a] = decompose(a);
        auto [trans_b, rot_b, scale_b] = decompose(b);

        Matrix3 identity = Matrix3::identity();
        Matrix3 rot_result = identity;

        float cos_angle = (rot_a.m[0][0] + rot_a.m[1][1] + rot_a.m[2][2] - 1.0f) * 0.5f;
        cos_angle = (cos_angle < -1.0f) ? -1.0f : (cos_angle > 1.0f) ? 1.0f : cos_angle;
        float angle = std::acos(cos_angle);

        if (angle < NORMALIZE_TOLERANCE_32) {
            rot_result = rot_a;
        } else {
            for (int col = 0; col < 3; ++col) {
                for (int row = 0; row < 3; ++row) {
                    rot_result.m[col][row] = rot_a.m[col][row] + t * (rot_b.m[col][row] - rot_a.m[col][row]);
                }
            }
            for (int col = 0; col < 3; ++col) {
                Vector3 c{rot_result.m[col][0], rot_result.m[col][1], rot_result.m[col][2]};
                c = c.normalize();
                rot_result.m[col][0] = c.x;
                rot_result.m[col][1] = c.y;
                rot_result.m[col][2] = c.z;
            }
        }

        Vector3 trans_result = Vector3::lerp(trans_a, trans_b, t);
        Vector3 scale_result = Vector3::lerp(scale_a, scale_b, t);

        return compose(trans_result, rot_result, scale_result);
    }

    auto Matrix4::lerp(const Matrix4& a, const Matrix4& b, float t) noexcept -> Matrix4 {
        Matrix4 result;
        for (int col = 0; col < 4; ++col) {
            for (int row = 0; row < 4; ++row) {
                result.m[col][row] = a.m[col][row] + t * (b.m[col][row] - a.m[col][row]);
            }
        }
        return result;
    }

    auto Matrix4::compose(const Vector3& translation, const Matrix3& rotation, const Vector3& scale) noexcept -> Matrix4 {
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

    auto Matrix4::compose(const Vector3& translation, const Matrix3& rotation) noexcept -> Matrix4 {
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

    auto Matrix4::perspective(float fov, float aspect, float near, float far) noexcept -> Matrix4 {
        float tan_half_fov = 1.0f / std::tan(fov * 0.5f);

        Matrix4 result = zero();
        result.m[0][0] = 1.0f / (aspect * tan_half_fov);
        result.m[1][1] = 1.0f / tan_half_fov;
        result.m[2][2] = -(far + near) / (far - near);
        result.m[2][3] = -1.0f;
        result.m[3][2] = -(2.0f * far * near) / (far - near);
        return result;
    }

    auto Matrix4::orthographic(float left, float right, float bottom, float top, float near, float far) noexcept -> Matrix4 {
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

    auto Matrix4::look_at(const Vector3& eye, const Vector3& target, const Vector3& up) noexcept -> Matrix4 {
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

    auto Matrix4::to_string() const noexcept -> std::string {
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(4);
        for (int row = 0; row < 4; ++row) {
            oss << "| ";
            for (int col = 0; col < 4; ++col) {
                oss << std::setw(8) << m[col][row] << " ";
            }
            oss << "|\n";
        }
        return oss.str();
    }

    auto Matrix4::print() const noexcept -> void {
        printf("%s", to_string().c_str());
    }

    auto Matrix4::inverse(const Matrix4& mat) noexcept -> std::optional<Matrix4> {
        float det = determinant(mat);
        if (std::abs(det) < NORMALIZE_TOLERANCE_32) {
            return std::nullopt;
        }

        // Use adjugate matrix: inv(M) = adj(M) / det(M)
        Matrix4 adj = adjugate(mat);
        float inv_det = 1.0f / det;

        Matrix4 result;
        for (int col = 0; col < 4; ++col) {
            for (int row = 0; row < 4; ++row) {
                result.m[col][row] = adj.m[col][row] * inv_det;
            }
        }
        return result;
    }

    auto Matrix4::transform_plane(const Matrix4& m, const Vector4& plane) noexcept -> Vector4 {
        Matrix4 inv = inverse(m).value_or(identity());
        Vector4 result = transpose(inv) * plane;
        return result.normalize();
    }

    auto Matrix4::transform_normal(const Matrix4& m, const Vector3& normal) noexcept -> Vector3 {
        Matrix4 inv = inverse(m).value_or(identity());
        Matrix4 inv_t = transpose(inv);
        Vector4 n{normal.x, normal.y, normal.z, 0.0f};
        Vector4 result = inv_t * n;
        return Vector3{result.x, result.y, result.z}.normalize();
    }

    auto Matrix4::transform_point(const Matrix4& m, const Vector3& point) noexcept -> Vector3 {
        Vector4 p{point.x, point.y, point.z, 1.0f};
        Vector4 result = m * p;
        return {result.x, result.y, result.z};
    }

    auto Matrix4::transform_vector(const Matrix4& m, const Vector3& vector) noexcept -> Vector3 {
        Vector4 v{vector.x, vector.y, vector.z, 0.0f};
        Vector4 result = m * v;
        return {result.x, result.y, result.z};
    }

    auto Matrix4::inverse_transform_point(const Matrix4& m, const Vector3& point) noexcept -> Vector3 {
        auto inv = inverse(m);
        return inv ? transform_point(*inv, point) : point;
    }

    auto Matrix4::inverse_transform_vector(const Matrix4& m, const Vector3& vector) noexcept -> Vector3 {
        auto inv = inverse(m);
        return inv ? transform_vector(*inv, vector) : vector;
    }

    auto Matrix4::from_rows(const Vector4& row0, const Vector4& row1, const Vector4& row2, const Vector4& row3) noexcept -> Matrix4 {
        Matrix4 result;
        result.m[0][0] = row0.x; result.m[1][0] = row0.y; result.m[2][0] = row0.z; result.m[3][0] = row0.w;
        result.m[0][1] = row1.x; result.m[1][1] = row1.y; result.m[2][1] = row1.z; result.m[3][1] = row1.w;
        result.m[0][2] = row2.x; result.m[1][2] = row2.y; result.m[2][2] = row2.z; result.m[3][2] = row2.w;
        result.m[0][3] = row3.x; result.m[1][3] = row3.y; result.m[2][3] = row3.z; result.m[3][3] = row3.w;
        return result;
    }

    auto Matrix4::from_columns(const Vector4& col0, const Vector4& col1, const Vector4& col2, const Vector4& col3) noexcept -> Matrix4 {
        Matrix4 result;
        result.m[0][0] = col0.x; result.m[0][1] = col0.y; result.m[0][2] = col0.z; result.m[0][3] = col0.w;
        result.m[1][0] = col1.x; result.m[1][1] = col1.y; result.m[1][2] = col1.z; result.m[1][3] = col1.w;
        result.m[2][0] = col2.x; result.m[2][1] = col2.y; result.m[2][2] = col2.z; result.m[2][3] = col2.w;
        result.m[3][0] = col3.x; result.m[3][1] = col3.y; result.m[3][2] = col3.z; result.m[3][3] = col3.w;
        return result;
    }

    auto Matrix4::rotation_between_vectors(const Vector3& from, const Vector3& to) noexcept -> Matrix4 {
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

    auto Matrix4::reflect(const Vector4& plane) noexcept -> Matrix4 {
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

    auto Matrix4::determinant(const Matrix4& mat) noexcept -> float {
        float a = mat.m[0][0], b = mat.m[1][0], c = mat.m[2][0], d = mat.m[3][0];
        float e = mat.m[0][1], f = mat.m[1][1], g = mat.m[2][1], h = mat.m[3][1];
        float i = mat.m[0][2], j = mat.m[1][2], k = mat.m[2][2], l = mat.m[3][2];
        float m_ = mat.m[0][3], n = mat.m[1][3], o = mat.m[2][3], p = mat.m[3][3];

        float C00 = f * (k * p - l * o) - g * (j * p - l * n) + h * (j * o - k * n);
        float C01 = e * (k * p - l * o) - g * (i * p - l * m_) + h * (i * o - k * m_);
        float C02 = e * (j * p - l * n) - f * (i * p - l * m_) + h * (i * n - j * m_);
        float C03 = e * (j * o - k * n) - f * (i * o - k * m_) + g * (i * n - j * m_);

        return a * C00 - b * C01 + c * C02 - d * C03;
    }

    auto Matrix4::minor(const Matrix4& mat, int row, int col) noexcept -> float {
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

    auto Matrix4::cofactor(const Matrix4& mat, int row, int col) noexcept -> float {
        float m = minor(mat, row, col);
        return ((row + col) % 2 == 0) ? m : -m;
    }

    auto Matrix4::adjugate(const Matrix4& mat) noexcept -> Matrix4 {
        Matrix4 result;
        for (int row = 0; row < 4; ++row) {
            for (int col = 0; col < 4; ++col) {
                result.m[col][row] = cofactor(mat, row, col);
            }
        }
        return result;
    }

    auto Matrix4::get_quaternion(const Matrix4& mat) noexcept -> Vector4 {
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

    auto Matrix4::set_quaternion(Matrix4& mat, const Vector4& quat) noexcept -> Matrix4 {
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

    auto Matrix4::is_affine() const noexcept -> bool {
        return std::abs(m[0][3]) < NORMALIZE_TOLERANCE_32
            && std::abs(m[1][3]) < NORMALIZE_TOLERANCE_32
            && std::abs(m[2][3]) < NORMALIZE_TOLERANCE_32
            && std::abs(m[3][3] - 1.0f) < NORMALIZE_TOLERANCE_32;
    }

    auto Matrix4::is_orthogonal() const noexcept -> bool {
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

    auto Matrix4::is_orthonormal() const noexcept -> bool {
        Matrix3 r = get_rotation(*this);
        for (auto & col : r.m) {
            Vector3 c{col[0], col[1], col[2]};
            if (std::abs(c.length_sq() - 1.0f) > NORMALIZE_TOLERANCE_32) {
                return false;
            }
        }
        return is_orthogonal();
    }

    auto Matrix4::normalize_rotation() noexcept -> Matrix4& {
        for (int col = 0; col < 3; ++col) {
            Vector3 c{m[col][0], m[col][1], m[col][2]};
            c = c.normalize();
            m[col][0] = c.x;
            m[col][1] = c.y;
            m[col][2] = c.z;
        }
        return *this;
    }

    auto Matrix4::translate(const Vector3& translation) noexcept -> Matrix4 {
        Matrix4 result = identity();
        result.m[3][0] = translation.x;
        result.m[3][1] = translation.y;
        result.m[3][2] = translation.z;
        return result;
    }

    auto Matrix4::scale(const Vector3& scale) noexcept -> Matrix4 {
        Matrix4 result = identity();
        result.m[0][0] = scale.x;
        result.m[1][1] = scale.y;
        result.m[2][2] = scale.z;
        return result;
    }

    auto Matrix4::rotate(float angle, const Vector3& axis) noexcept -> Matrix4 {
        Vector3 n = axis.normalize();
        float c = std::cos(angle);
        float s = std::sin(angle);
        float t = 1.0f - c;

        Matrix4 result = identity();
        result.m[0][0] = t * n.x * n.x + c;
        result.m[0][1] = t * n.x * n.y + s * n.z;
        result.m[0][2] = t * n.x * n.z - s * n.y;

        result.m[1][0] = t * n.x * n.y - s * n.z;
        result.m[1][1] = t * n.y * n.y + c;
        result.m[1][2] = t * n.y * n.z + s * n.x;

        result.m[2][0] = t * n.x * n.z + s * n.y;
        result.m[2][1] = t * n.y * n.z - s * n.x;
        result.m[2][2] = t * n.z * n.z + c;

        return result;
    }

    auto Matrix4::rotate_x(float angle) noexcept -> Matrix4 {
        float c = std::cos(angle);
        float s = std::sin(angle);

        Matrix4 result = identity();
        result.m[1][1] = c;
        result.m[1][2] = s;
        result.m[2][1] = -s;
        result.m[2][2] = c;
        return result;
    }

    auto Matrix4::rotate_y(float angle) noexcept -> Matrix4 {
        float c = std::cos(angle);
        float s = std::sin(angle);

        Matrix4 result = identity();
        result.m[0][0] = c;
        result.m[0][2] = -s;
        result.m[2][0] = s;
        result.m[2][2] = c;
        return result;
    }

    auto Matrix4::rotate_z(float angle) noexcept -> Matrix4 {
        float c = std::cos(angle);
        float s = std::sin(angle);

        Matrix4 result = identity();
        result.m[0][0] = c;
        result.m[0][1] = s;
        result.m[1][0] = -s;
        result.m[1][1] = c;
        return result;
    }

} // namespace gkit::math