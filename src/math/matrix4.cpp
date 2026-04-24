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

} // namespace gkit::math