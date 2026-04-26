#include "gkit/math/matrix3.hpp"

namespace gkit::math {

    auto Matrix3::operator*(const Matrix3& other) const noexcept -> Matrix3 {
        Matrix3 result;
        for (int col = 0; col < 3; ++col) {
            for (int row = 0; row < 3; ++row) {
                result.m[col][row] = 0.0f;
                for (int k = 0; k < 3; ++k) {
                    result.m[col][row] += this->m[k][row] * other.m[col][k];
                }
            }
        }
        return result;
    }

    auto Matrix3::operator*(const Vector3& v) const noexcept -> Vector3 {
        return {
            m[0][0] * v.x + m[1][0] * v.y + m[2][0] * v.z,
            m[0][1] * v.x + m[1][1] * v.y + m[2][1] * v.z,
            m[0][2] * v.x + m[1][2] * v.y + m[2][2] * v.z
        };
    }

    auto Matrix3::transpose(const Matrix3& mat) noexcept -> Matrix3 {
        Matrix3 result;
        for (int col = 0; col < 3; ++col) {
            for (int row = 0; row < 3; ++row) {
                result.m[col][row] = mat.m[row][col];
            }
        }
        return result;
    }

    auto Matrix3::determinant(const Matrix3& mat) noexcept -> float {
        float a = mat.m[0][0], b = mat.m[1][0], c = mat.m[2][0];
        float d = mat.m[0][1], e = mat.m[1][1], f = mat.m[2][1];
        float g = mat.m[0][2], h = mat.m[1][2], i = mat.m[2][2];

        return a * (e * i - f * h)
             - b * (d * i - f * g)
             + c * (d * h - e * g);
    }

    auto Matrix3::inverse(const Matrix3& mat) noexcept -> std::optional<Matrix3> {
        float det = determinant(mat);
        if (std::abs(det) < NORMALIZE_TOLERANCE_32) {
            return std::nullopt;
        }

        float inv_det = 1.0f / det;
        Matrix3 result;

        float a = mat.m[0][0], b = mat.m[1][0], c = mat.m[2][0];
        float d = mat.m[0][1], e = mat.m[1][1], f = mat.m[2][1];
        float g = mat.m[0][2], h = mat.m[1][2], i = mat.m[2][2];

        result.m[0][0] = (e * i - f * h) * inv_det;
        result.m[1][0] = (c * h - b * i) * inv_det;
        result.m[2][0] = (b * f - c * e) * inv_det;
        result.m[0][1] = (f * g - d * i) * inv_det;
        result.m[1][1] = (a * i - c * g) * inv_det;
        result.m[2][1] = (c * d - a * f) * inv_det;
        result.m[0][2] = (d * h - e * g) * inv_det;
        result.m[1][2] = (b * g - a * h) * inv_det;
        result.m[2][2] = (a * e - b * d) * inv_det;

        return result;
    }

    auto Matrix3::from_euler(float pitch, float yaw, float roll) noexcept -> Matrix3 {
        float cp = std::cos(pitch), sp = std::sin(pitch);
        float cy = std::cos(yaw), sy = std::sin(yaw);
        float cr = std::cos(roll), sr = std::sin(roll);

        Matrix3 result;
        result.m[0][0] = cy * cr + sy * sp * sr;
        result.m[0][1] = sr * cp;
        result.m[0][2] = -sy * cr + cy * sp * sr;
        result.m[1][0] = -cy * sr + sy * sp * cr;
        result.m[1][1] = cr * cp;
        result.m[1][2] = sy * sr + cy * sp * cr;
        result.m[2][0] = sy * cp;
        result.m[2][1] = -sp;
        result.m[2][2] = cy * cp;
        return result;
    }

    auto Matrix3::rotation_x(float angle) noexcept -> Matrix3 {
        float c = std::cos(angle), s = std::sin(angle);
        Matrix3 result;
        result.m[0][0] = 1.0f; result.m[0][1] = 0.0f; result.m[0][2] = 0.0f;
        result.m[1][0] = 0.0f; result.m[1][1] = c;    result.m[1][2] = s;
        result.m[2][0] = 0.0f; result.m[2][1] = -s;   result.m[2][2] = c;
        return result;
    }

    auto Matrix3::rotation_y(float angle) noexcept -> Matrix3 {
        float c = std::cos(angle), s = std::sin(angle);
        Matrix3 result;
        result.m[0][0] = c;    result.m[0][1] = 0.0f; result.m[0][2] = -s;
        result.m[1][0] = 0.0f; result.m[1][1] = 1.0f; result.m[1][2] = 0.0f;
        result.m[2][0] = s;    result.m[2][1] = 0.0f; result.m[2][2] = c;
        return result;
    }

    auto Matrix3::rotation_z(float angle) noexcept -> Matrix3 {
        float c = std::cos(angle), s = std::sin(angle);
        Matrix3 result;
        result.m[0][0] = c;    result.m[0][1] = s;    result.m[0][2] = 0.0f;
        result.m[1][0] = -s;   result.m[1][1] = c;    result.m[1][2] = 0.0f;
        result.m[2][0] = 0.0f; result.m[2][1] = 0.0f; result.m[2][2] = 1.0f;
        return result;
    }

    auto Matrix3::scaling(float scale) noexcept -> Matrix3 {
        return from_diagonal(scale);
    }

    auto Matrix3::scaling(float sx, float sy, float sz) noexcept -> Matrix3 {
        Matrix3 result;
        result.m[0][0] = sx; result.m[1][1] = sy; result.m[2][2] = sz;
        return result;
    }

} // namespace gkit::math