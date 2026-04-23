#include "gkit/math/constants.hpp"
#include <gkit/math/matrix4.hpp>
#include <gkit/math/vector3.hpp>
#include <gkit/math/vector4.hpp>
#include <gkit/math/matrix3.hpp>
#include <string>
#include <sstream>
#include <iomanip>
#include <format>
#include <iostream>

using gkit::math::Matrix4;
using gkit::math::Matrix3;
using gkit::math::Vector3;
using gkit::math::Vector4;

auto mat4_str(const Matrix4& mat) -> std::string {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(3);
    for (int row = 0; row < 4; ++row) {
        oss << "| ";
        for (int col = 0; col < 4; ++col) {
            oss << std::setw(8) << mat.m[col][row] << " ";
        }
        oss << "|\n";
    }
    return oss.str();
}

auto mat3_str(const Matrix3& mat) -> std::string {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(3);
    for (int row = 0; row < 3; ++row) {
        oss << "| ";
        for (int col = 0; col < 3; ++col) {
            oss << std::setw(8) << mat.m[col][row] << " ";
        }
        oss << "|\n";
    }
    return oss.str();
}

auto vec3_str(const Vector3& v) -> std::string {
    return std::format("({:.3f}, {:.3f}, {:.3f})", v.x, v.y, v.z);
}

auto vec4_str(const Vector4& v) -> std::string {
    return std::format("({:.3f}, {:.3f}, {:.3f}, {:.3f})", v.x, v.y, v.z, v.w);
}

auto main() -> int {
    std::cout << "=== Matrix4 Test ===" << std::endl;

    // Test 1: Identity matrix
    std::cout << "\n1. Identity matrix:" << std::endl;
    auto identity = Matrix4::identity();
    std::cout << mat4_str(identity) << std::endl;

    // Test 2: Zero matrix
    std::cout << "\n2. Zero matrix:" << std::endl;
    auto zero = Matrix4::zero();
    std::cout << mat4_str(zero) << std::endl;

    // Test 3: Diagonal matrix
    std::cout << "\n3. Diagonal matrix (3.0f):" << std::endl;
    auto diag = Matrix4(3.0f);
    std::cout << mat4_str(diag) << std::endl;

    // Test 4: Identity * Vec4
    std::cout << "\n4. Identity * Vec4:" << std::endl;
    Vector4 v4{1.0f, 2.0f, 3.0f, 1.0f};
    auto result4 = identity * v4;
    std::cout << "v4 = " << vec4_str(v4) << std::endl;
    std::cout << "result = " << vec4_str(result4) << std::endl;

    // Test 5: Identity * Vec3
    std::cout << "\n5. Identity * Vec3:" << std::endl;
    Vector3 v3{1.0f, 2.0f, 3.0f};
    auto result3 = identity * v3;
    std::cout << "v3 = " << vec3_str(v3) << std::endl;
    std::cout << "result = " << vec3_str(result3) << std::endl;

    // Test 6: Determinant of identity
    std::cout << "\n6. Determinant test:" << std::endl;
    std::cout << "det(identity) = " << Matrix4::determinant(identity) << " (expected: 1)" << std::endl;

    // Test 7: Translation matrix
    std::cout << "\n7. Translation (2, 3, 4):" << std::endl;
    auto translate = Matrix4::translate({2.0f, 3.0f, 4.0f});
    std::cout << mat4_str(translate) << std::endl;

    // Test 8: Transform point with translation
    std::cout << "\n8. Transform point:" << std::endl;
    auto point = Matrix4::transform_point(translate, {1.0f, 1.0f, 1.0f});
    std::cout << "translate * (1,1,1) = " << vec3_str(point) << " (expected: (3,4,5))" << std::endl;

    // Test 9: Transform vector (no translation)
    std::cout << "\n9. Transform vector:" << std::endl;
    auto vec = Matrix4::transform_vector(translate, {1.0f, 0.0f, 0.0f});
    std::cout << "translate * (1,0,0) = " << vec3_str(vec) << " (expected: (1,0,0))" << std::endl;

    // Test 10: Scale matrix
    std::cout << "\n10. Scale (2, 3, 4):" << std::endl;
    auto scale = Matrix4::scale({2.0f, 3.0f, 4.0f});
    std::cout << mat4_str(scale) << std::endl;

    // Test 11: Scale transform
    std::cout << "\n11. Scale transform:" << std::endl;
    auto scaled_point = Matrix4::transform_point(scale, {2.0f, 2.0f, 2.0f});
    std::cout << "scale * (2,2,2) = " << vec3_str(scaled_point) << " (expected: (4,6,8))" << std::endl;

    // Test 12: Rotation X (90 degrees)
    std::cout << "\n12. Rotation X (90 degrees):" << std::endl;
    auto rot_x = Matrix4::rotate_x(gkit::math::PI_32 / 2.0f);
    std::cout << mat4_str(rot_x) << std::endl;

    // Verify rotation
    std::cout << "rot_x * (0,1,0) = " << vec3_str(Matrix4::transform_point(rot_x, {0.0f, 1.0f, 0.0f})) << " (expected: ~ (0,0,1))" << std::endl;

    // Test 13: Rotation Y
    std::cout << "\n13. Rotation Y (90 degrees):" << std::endl;
    auto rot_y = Matrix4::rotate_y(gkit::math::PI_32 / 2.0f);
    std::cout << mat4_str(rot_y) << std::endl;
    std::cout << "rot_y * (1,0,0) = " << vec3_str(Matrix4::transform_point(rot_y, {1.0f, 0.0f, 0.0f})) << " (expected: ~ (0,0,-1))" << std::endl;

    // Test 14: Rotation Z
    std::cout << "\n14. Rotation Z (90 degrees):" << std::endl;
    auto rot_z = Matrix4::rotate_z(gkit::math::PI_32 / 2.0f);
    std::cout << mat4_str(rot_z) << std::endl;
    std::cout << "rot_z * (1,0,0) = " << vec3_str(Matrix4::transform_point(rot_z, {1.0f, 0.0f, 0.0f})) << " (expected: ~ (0,1,0))" << std::endl;

    // Test 15: Arbitrary axis rotation
    std::cout << "\n15. Rotation around (1,1,1) axis 90 degrees:" << std::endl;
    auto rot_axis = Matrix4::rotate(gkit::math::PI_32 / 2.0f, Vector3{1.0f, 1.0f, 1.0f}.normalize());
    std::cout << mat4_str(rot_axis) << std::endl;

    // Test 16: Transpose
    std::cout << "\n16. Transpose test:" << std::endl;
    auto transposed = Matrix4::transpose(scale);
    std::cout << "transpose(scale):" << std::endl;
    std::cout << mat4_str(transposed) << std::endl;

    // Test 17: Matrix multiplication
    std::cout << "\n17. Matrix multiplication (T * R * S):" << std::endl;
    auto TRS = translate * rot_x * scale;
    std::cout << mat4_str(TRS) << std::endl;

    // Test 18: Inverse of identity
    std::cout << "\n18. Inverse test:" << std::endl;
    auto inv_identity = Matrix4::inverse(identity);
    if (inv_identity.has_value()) {
        std::cout << "inverse(identity) exists" << std::endl;
    }

    // Test 19: Inverse of TRS
    std::cout << "\n19. Inverse of TRS matrix:" << std::endl;
    auto inv_trs = Matrix4::inverse(TRS);
    if (inv_trs.has_value()) {
        std::cout << "inverse(TRS) exists" << std::endl;
        auto composed = TRS * inv_trs.value();
        std::cout << "TRS * inverse(TRS):" << std::endl;
        std::cout << mat4_str(composed) << std::endl;
    } else {
        std::cout << "TRS is singular (no inverse)" << std::endl;
    }

    // Test 20: Get/Set identity
    std::cout << "\n20. Set identity:" << std::endl;
    auto mat = Matrix4::zero();
    mat.set_identity();
    std::cout << "After set_identity():" << std::endl;
    std::cout << mat4_str(mat) << std::endl;

    // Test 21: Extract translation
    std::cout << "\n21. Extract translation from TRS:" << std::endl;
    auto extracted_trans = Matrix4::get_translation(TRS);
    std::cout << "Translation = " << vec3_str(extracted_trans) << " (expected: (2,3,4))" << std::endl;

    // Test 22: Extract rotation
    std::cout << "\n22. Extract rotation from TRS:" << std::endl;
    auto extracted_rot = Matrix4::get_rotation(TRS);
    std::cout << "Rotation:" << std::endl;
    std::cout << mat3_str(extracted_rot) << std::endl;

    // Test 23: Extract scale
    std::cout << "\n23. Extract scale from TRS:" << std::endl;
    auto extracted_scale = Matrix4::get_scale(TRS);
    std::cout << "Scale = " << vec3_str(extracted_scale) << std::endl;

    // Test 24: Decompose
    std::cout << "\n24. Decompose TRS:" << std::endl;
    auto [t, r, s] = Matrix4::decompose(TRS);
    std::cout << "Translation = " << vec3_str(t) << std::endl;
    std::cout << "Scale = " << vec3_str(s) << std::endl;

    // Test 25: Compose TRS
    std::cout << "\n25. Compose TRS:" << std::endl;
    auto composed_trs = Matrix4::compose(extracted_trans, extracted_rot, extracted_scale);
    std::cout << mat4_str(composed_trs) << std::endl;

    // Test 26: Perspective projection
    std::cout << "\n26. Perspective projection (fov=60deg, aspect=1.778, near=0.1, far=100):" << std::endl;
    auto persp = Matrix4::perspective(gkit::math::PI_32 / 3.0f, 16.0f/9.0f, 0.1f, 100.0f);
    std::cout << mat4_str(persp) << std::endl;

    // Test 27: Orthographic projection
    std::cout << "\n27. Orthographic projection (left=-10, right=10, bottom=-10, top=10, near=0.1, far=100):" << std::endl;
    auto ortho = Matrix4::orthographic(-10.0f, 10.0f, -10.0f, 10.0f, 0.1f, 100.0f);
    std::cout << mat4_str(ortho) << std::endl;

    // Test 28: Look-at
    std::cout << "\n28. Look-at (eye=(0,0,5), target=(0,0,0), up=(0,1,0)):" << std::endl;
    auto look = Matrix4::look_at({0.0f, 0.0f, 5.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f});
    std::cout << mat4_str(look) << std::endl;

    // Test 29: Is affine
    std::cout << "\n29. Is affine check:" << std::endl;
    std::cout << "identity.is_affine() = " << (identity.is_affine() ? "true" : "false") << " (expected: true)" << std::endl;
    std::cout << "persp.is_affine() = " << (persp.is_affine() ? "true" : "false") << " (expected: false)" << std::endl;

    // Test 30: Is orthonormal
    std::cout << "\n30. Is orthonormal check:" << std::endl;
    std::cout << "rot_x.is_orthonormal() = " << (rot_x.is_orthonormal() ? "true" : "false") << " (expected: true)" << std::endl;

    // Test 31: Trace
    std::cout << "\n31. Trace:" << std::endl;
    std::cout << "trace(identity) = " << identity.trace() << " (expected: 4)" << std::endl;

    // Test 32: From rows/columns
    std::cout << "\n32. From rows:" << std::endl;
    auto from_rows = Matrix4::from_rows(
        {1.0f, 0.0f, 0.0f, 0.0f},
        {0.0f, 2.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 3.0f, 0.0f},
        {4.0f, 5.0f, 6.0f, 1.0f}
    );
    std::cout << mat4_str(from_rows) << std::endl;

    // Test 33: Lerp
    std::cout << "\n33. Lerp:" << std::endl;
    auto lerp_result = Matrix4::lerp(Matrix4::zero(), Matrix4::identity(), 0.5f);
    std::cout << "lerp(zero, identity, 0.5):" << std::endl;
    std::cout << mat4_str(lerp_result) << std::endl;

    // Test 34: Shear
    std::cout << "\n34. Shear (1, 2, 3):" << std::endl;
    auto shear = Matrix4::shear({1.0f, 2.0f, 3.0f});
    std::cout << mat4_str(shear) << std::endl;

    // Test 35: Row/Column access
    std::cout << "\n35. Row/Column access:" << std::endl;
    auto row0 = identity.row(0);
    auto col0 = identity.column(0);
    std::cout << "identity.row(0) = " << vec4_str(row0) << " (expected: (1,0,0,0))" << std::endl;
    std::cout << "identity.column(0) = " << vec4_str(col0) << " (expected: (1,0,0,0))" << std::endl;

    // Test 36: Data pointer
    std::cout << "\n36. Data pointer:" << std::endl;
    const float* data_ptr = identity.data();
    std::cout << "identity.data()[0] = " << data_ptr[0] << " (expected: 1)" << std::endl;
    std::cout << "identity.data()[5] = " << data_ptr[5] << " (expected: 1, diagonal element)" << std::endl;

    // Test 37: To string
    std::cout << "\n37. To string:" << std::endl;
    std::cout << identity.to_string() << std::endl;

    // Test 38: Compound operators
    std::cout << "\n38. Compound operators:" << std::endl;
    auto compound = Matrix4::identity();
    compound += Matrix4(1.0f);
    std::cout << "identity += Matrix4(1):" << std::endl;
    std::cout << mat4_str(compound) << std::endl;

    compound *= 2.0f;
    std::cout << "after *= 2:" << std::endl;
    std::cout << mat4_str(compound) << std::endl;

    // Test 39: Rotation between vectors
    std::cout << "\n39. Rotation between vectors:" << std::endl;
    auto rot_between = Matrix4::rotation_between_vectors({1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f});
    std::cout << mat4_str(rot_between) << std::endl;
    std::cout << "rot_between * (1,0,0) = " << vec3_str(Matrix4::transform_vector(rot_between, {1.0f, 0.0f, 0.0f})) << std::endl;

    // Test 40: Get quaternion
    std::cout << "\n40. Get quaternion from rotation:" << std::endl;
    auto quat = Matrix4::get_quaternion(rot_x);
    std::cout << "Quaternion from rot_x(90deg): " << vec4_str(quat) << std::endl;

    std::cout << "\n=== All tests completed! ===" << std::endl;
    return 0;
}