#include "gkit/math/constants.hpp"

#include <format>
#include <iostream>
#include <string>

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

auto main() -> int {
    std::cout << "=== Matrix3 Test ===" << '\n';

    // Test 1: Identity matrix
    std::cout << "\n1. Identity matrix:" << '\n';
    auto identity = Matrix3::identity();
    std::cout << mat_str(identity) << '\n';

    // Test 2: Matrix * Vector (should preserve vector)
    std::cout << "\n2. Identity * Vector:" << '\n';
    Vector3 v{1.0f, 2.0f, 3.0f};
    auto result = identity * v;
    std::cout << "v = (1, 2, 3)" << '\n';
    std::cout << "result = (" << result.x << ", " << result.y << ", " << result.z << ")" << '\n';

    // Test 3: Determinant of identity should be 1
    std::cout << "\n3. Determinant test:" << '\n';
    std::cout << "det(identity) = " << Matrix3::determinant(identity) << " (expected: 1)" << '\n';

    // Test 4: Rotation matrix (90 degrees around X)
    std::cout << "\n4. Rotation X (90 degrees):" << '\n';
    auto rot_x = Matrix3::rotation_x(gkit::math::PI_32 / 2.0f);
    std::cout << mat_str(rot_x) << '\n';

    // Verify: Rotating (0, 1, 0) around X by 90 degrees should give (0, 0, 1)
    Vector3 up{0.0f, 1.0f, 0.0f};
    auto rotated = rot_x * up;
    std::cout << "rot_x * (0,1,0) = (" << rotated.x << ", " << rotated.y << ", " << rotated.z << ")" << '\n';
    std::cout << "expected: (0, 0, 1)" << '\n';

    // Test 5: Rotation matrix determinant should be 1
    std::cout << "\n5. det(rotation_x) = " << Matrix3::determinant(rot_x) << " (expected: 1)" << '\n';

    // Test 6: Inverse of identity is identity
    std::cout << "\n6. Inverse test:" << '\n';
    auto inv_identity = Matrix3::inverse(identity);
    if (inv_identity.has_value()) {
        std::cout << "inverse(identity):" << '\n';
        std::cout << mat_str(inv_identity.value()) << '\n';
    }

    // Test 7: Rotation * inverse should be identity
    auto inv_rot_x = Matrix3::inverse(rot_x);
    if (inv_rot_x.has_value()) {
        auto composed = rot_x * inv_rot_x.value();
        std::cout << "rotation_x * inverse(rotation_x):" << '\n';
        std::cout << mat_str(composed) << '\n';
    }

    // Test 8: Scaling matrix
    std::cout << "\n8. Scaling (2, 3, 4):" << '\n';
    auto scale = Matrix3::scaling(2.0f, 3.0f, 4.0f);
    std::cout << mat_str(scale) << '\n';

    // Test 9: Transpose
    std::cout << "\n9. Transpose test:" << '\n';
    auto transposed = Matrix3::transpose(scale);
    std::cout << "transpose(scale):" << '\n';
    std::cout << mat_str(transposed) << '\n';

    // Test 10: From diagonal
    std::cout << "\n10. From diagonal (5.0f):" << '\n';
    auto diag = Matrix3::from_diagonal(5.0f);
    std::cout << mat_str(diag) << '\n';

    // Test 11: Matrix multiplication
    std::cout << "\n11. Matrix multiplication:" << '\n';
    auto a = Matrix3::rotation_x(1.0f);
    auto b = Matrix3::rotation_y(1.0f);
    auto c = a * b;
    std::cout << "rotation_x(1) * rotation_y(1):" << '\n';
    std::cout << mat_str(c) << '\n';

    // Test 12: Orthogonality check (R * R^T = I)
    std::cout << "\n12. Orthogonality test (rotation matrix):" << '\n';
    auto r     = Matrix3::rotation_z(0.5f);
    auto rt    = Matrix3::transpose(r);
    auto ortho = r * rt;
    std::cout << "R * R^T:" << '\n';
    std::cout << mat_str(ortho) << '\n';

    std::cout << "\nAll tests completed!" << '\n';
    return 0;
}