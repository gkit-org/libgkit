#include "gkit/math/constants.hpp"
#include <gkit/math/matrix3.hpp>
#include <gkit/math/vector3.hpp>
#include <string>
#include <format>
#include <iostream>

using gkit::math::Matrix3;
using gkit::math::Vector3;

auto mat_str(const Matrix3& mat) -> std::string {
    auto [m00, m10, m20, m01, m11, m21, m02, m12, m22] = mat.properties();
    return std::format(
        "|, {:.3f} {:.3f} {:.3f} |\n|  {:.3f} {:.3f} {:.3f} |\n|  {:.3f} {:.3f} {:.3f} |",
        m00, m10, m20, m01, m11, m21, m02, m12, m22
    );
}

auto main() -> int {
    std::cout << "=== Matrix3 Test ===" << std::endl;

    // Test 1: Identity matrix
    std::cout << "\n1. Identity matrix:" << std::endl;
    auto identity = Matrix3::identity();
    std::cout << mat_str(identity) << std::endl;

    // Test 2: Matrix * Vector (should preserve vector)
    std::cout << "\n2. Identity * Vector:" << std::endl;
    Vector3 v{1.0f, 2.0f, 3.0f};
    auto result = identity * v;
    std::cout << "v = (1, 2, 3)" << std::endl;
    std::cout << "result = (" << result.x << ", " << result.y << ", " << result.z << ")" << std::endl;

    // Test 3: Determinant of identity should be 1
    std::cout << "\n3. Determinant test:" << std::endl;
    std::cout << "det(identity) = " << Matrix3::determinant(identity) << " (expected: 1)" << std::endl;

    // Test 4: Rotation matrix (90 degrees around X)
    std::cout << "\n4. Rotation X (90 degrees):" << std::endl;
    auto rot_x = Matrix3::rotation_x(gkit::math::PI_32 / 2.0f);
    std::cout << mat_str(rot_x) << std::endl;

    // Verify: Rotating (0, 1, 0) around X by 90 degrees should give (0, 0, 1)
    Vector3 up{0.0f, 1.0f, 0.0f};
    auto rotated = rot_x * up;
    std::cout << "rot_x * (0,1,0) = (" << rotated.x << ", " << rotated.y << ", " << rotated.z << ")" << std::endl;
    std::cout << "expected: (0, 0, 1)" << std::endl;

    // Test 5: Rotation matrix determinant should be 1
    std::cout << "\n5. det(rotation_x) = " << Matrix3::determinant(rot_x) << " (expected: 1)" << std::endl;

    // Test 6: Inverse of identity is identity
    std::cout << "\n6. Inverse test:" << std::endl;
    auto inv_identity = Matrix3::inverse(identity);
    if (inv_identity.has_value()) {
        std::cout << "inverse(identity):" << std::endl;
        std::cout << mat_str(inv_identity.value()) << std::endl;
    }

    // Test 7: Rotation * inverse should be identity
    auto inv_rot_x = Matrix3::inverse(rot_x);
    if (inv_rot_x.has_value()) {
        auto composed = rot_x * inv_rot_x.value();
        std::cout << "rotation_x * inverse(rotation_x):" << std::endl;
        std::cout << mat_str(composed) << std::endl;
    }

    // Test 8: Scaling matrix
    std::cout << "\n8. Scaling (2, 3, 4):" << std::endl;
    auto scale = Matrix3::scaling(2.0f, 3.0f, 4.0f);
    std::cout << mat_str(scale) << std::endl;

    // Test 9: Transpose
    std::cout << "\n9. Transpose test:" << std::endl;
    auto transposed = Matrix3::transpose(scale);
    std::cout << "transpose(scale):" << std::endl;
    std::cout << mat_str(transposed) << std::endl;

    // Test 10: From diagonal
    std::cout << "\n10. From diagonal (5.0f):" << std::endl;
    auto diag = Matrix3::from_diagonal(5.0f);
    std::cout << mat_str(diag) << std::endl;

    // Test 11: Matrix multiplication
    std::cout << "\n11. Matrix multiplication:" << std::endl;
    auto a = Matrix3::rotation_x(1.0f);
    auto b = Matrix3::rotation_y(1.0f);
    auto c = a * b;
    std::cout << "rotation_x(1) * rotation_y(1):" << std::endl;
    std::cout << mat_str(c) << std::endl;

    // Test 12: Orthogonality check (R * R^T = I)
    std::cout << "\n12. Orthogonality test (rotation matrix):" << std::endl;
    auto r = Matrix3::rotation_z(0.5f);
    auto rt = Matrix3::transpose(r);
    auto ortho = r * rt;
    std::cout << "R * R^T:" << std::endl;
    std::cout << mat_str(ortho) << std::endl;

    std::cout << "\nAll tests completed!" << std::endl;
    return 0;
}