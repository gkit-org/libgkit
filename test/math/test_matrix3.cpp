#include "test_utils.hpp"

#include <format>
#include <string>

#include <gkit/math/constants.hpp>
#include <gkit/math/matrix3.hpp>
#include <gkit/math/vector3.hpp>

using gkit::math::Matrix3;
using gkit::math::Vector3;

auto mat_str(const Matrix3& mat) -> std::string {
    auto [m00, m10, m20, m01, m11, m21, m02, m12, m22] = mat.properties();
    return std::format("|  {:.3f} {:.3f} {:.3f} |\n|  {:.3f} {:.3f} {:.3f} |\n|  {:.3f} {:.3f} {:.3f} |",
                       m00,
                       m10,
                       m20,
                       m01,
                       m11,
                       m21,
                       m02,
                       m12,
                       m22);
}


auto test_identity_and_vector() -> bool {
    gkit::test::logln("=== identity matrix ===");

    auto identity = Matrix3::identity();
    gkit::test::logln("  {}", mat_str(identity));

    Vector3 v{1.0f, 2.0f, 3.0f};
    auto result = identity * v;
    gkit::test::assert_if(result.x == 1.0f && result.y == 2.0f && result.z == 3.0f, "identity * vector failed");
    gkit::test::logln("  identity * (1, 2, 3) = ({}, {}, {})", result.x, result.y, result.z);

    gkit::test::assert_if(Matrix3::determinant(identity) == 1.0f, "det(identity) must be 1");
    gkit::test::logln("  det(identity) = {}", Matrix3::determinant(identity));
    return true;
}


auto test_rotation() -> bool {
    gkit::test::logln("=== rotation matrix ===");

    auto rot_x = Matrix3::rotation_x(gkit::math::PI_32 / 2.0f);
    gkit::test::logln("  {}", mat_str(rot_x));

    Vector3 up{0.0f, 1.0f, 0.0f};
    auto rotated = rot_x * up;
    gkit::test::logln("  rot_x * (0,1,0) = ({}, {}, {}) (expected: (0, 0, 1))", rotated.x, rotated.y, rotated.z);
    gkit::test::assert_if(std::abs(rotated.x) < 1e-4f && std::abs(rotated.y) < 1e-4f &&
                              std::abs(rotated.z - 1.0f) < 1e-4f,
                          "rotation_x(90) * (0,1,0) must be ~(0,0,1)");

    gkit::test::assert_if(std::abs(Matrix3::determinant(rot_x) - 1.0f) < 1e-4f, "det(rotation_x) must be 1");
    gkit::test::logln("  det(rotation_x) = {} (expected: 1)", Matrix3::determinant(rot_x));
    return true;
}


auto test_inverse() -> bool {
    gkit::test::logln("=== inverse ===");

    auto identity     = Matrix3::identity();
    auto inv_identity = Matrix3::inverse(identity);
    gkit::test::assert_if(inv_identity.has_value(), "inverse(identity) must have a value");
    gkit::test::logln("  inverse(identity): {}", mat_str(inv_identity.value()));

    auto rot_x     = Matrix3::rotation_x(gkit::math::PI_32 / 2.0f);
    auto inv_rot_x = Matrix3::inverse(rot_x);
    gkit::test::assert_if(inv_rot_x.has_value(), "inverse(rotation_x) must have a value");

    auto composed = rot_x * inv_rot_x.value();
    gkit::test::logln("  rotation_x * inverse(rotation_x): {}", mat_str(composed));
    auto [c00, c10, c20, c01, c11, c21, c02, c12, c22] = composed.properties();
    gkit::test::assert_if(std::abs(c00 - 1.0f) < 1e-4f && std::abs(c11 - 1.0f) < 1e-4f && std::abs(c22 - 1.0f) < 1e-4f,
                          "rotation * inverse must be identity");
    return true;
}


auto test_scale_transpose_diagonal() -> bool {
    gkit::test::logln("=== scale, transpose, diagonal ===");

    auto scale = Matrix3::scaling(2.0f, 3.0f, 4.0f);
    gkit::test::logln("  {}", mat_str(scale));

    auto transposed = Matrix3::transpose(scale);
    gkit::test::logln("  transpose(scale): {}", mat_str(transposed));
    gkit::test::assert_if(std::abs(transposed.m[0][0] - 2.0f) < 1e-4f && std::abs(transposed.m[1][1] - 3.0f) < 1e-4f &&
                              std::abs(transposed.m[2][2] - 4.0f) < 1e-4f,
                          "transpose of diagonal must be itself");

    auto diag = Matrix3::from_diagonal(5.0f);
    gkit::test::logln("  from_diagonal(5.0f): {}", mat_str(diag));
    auto [d00, d10, d20, d01, d11, d21, d02, d12, d22] = diag.properties();
    gkit::test::assert_if(d00 == 5.0f && d11 == 5.0f && d22 == 5.0f, "from_diagonal failed");
    return true;
}


auto test_multiplication_and_orthogonality() -> bool {
    gkit::test::logln("=== matrix multiplication and orthogonality ===");

    auto a = Matrix3::rotation_x(1.0f);
    auto b = Matrix3::rotation_y(1.0f);
    auto c = a * b;
    gkit::test::logln("  rotation_x(1) * rotation_y(1): {}", mat_str(c));

    auto r     = Matrix3::rotation_z(0.5f);
    auto rt    = Matrix3::transpose(r);
    auto ortho = r * rt;
    gkit::test::logln("  R * R^T: {}", mat_str(ortho));

    auto [m00, m10, m20, m01, m11, m21, m02, m12, m22] = ortho.properties();
    gkit::test::assert_if(std::abs(m00 - 1.0f) < 1e-4f && std::abs(m11 - 1.0f) < 1e-4f && std::abs(m22 - 1.0f) < 1e-4f,
                          "rotation matrix must be orthonormal");
    return true;
}


auto main() -> int {
    auto test_runner = gkit::test::TestRunner()
                           .add_test_func(test_identity_and_vector)
                           .add_test_func(test_rotation)
                           .add_test_func(test_inverse)
                           .add_test_func(test_scale_transpose_diagonal)
                           .add_test_func(test_multiplication_and_orthogonality);

    test_runner.run();
    return 0;
}
