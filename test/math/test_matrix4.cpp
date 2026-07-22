#include "gkit/math/constants.hpp"

#include <format>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

#include <gkit/math/matrix3.hpp>
#include <gkit/math/matrix4.hpp>
#include <gkit/math/vector3.hpp>
#include <gkit/math/vector4.hpp>

auto mat4_str(const gkit::math::Matrix4& mat) -> std::string {
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

auto mat3_str(const gkit::math::Matrix3& mat) -> std::string {
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

auto vec3_str(const gkit::math::Vector3& v) -> std::string {
    return std::format("({:.3f}, {:.3f}, {:.3f})", v.x, v.y, v.z);
}

auto vec4_str(const gkit::math::Vector4& v) -> std::string {
    return std::format("({:.3f}, {:.3f}, {:.3f}, {:.3f})", v.x, v.y, v.z, v.w);
}

auto main() -> int {
    using namespace gkit::math; // NOLINT(google-build-using-namespace
    std::cout << "=== Matrix4 Test ===" << '\n';

    // Test 1: Identity matrix
    std::cout << "\n1. Identity matrix:" << '\n';
    auto identity = Matrix4::identity();
    std::cout << mat4_str(identity) << '\n';

    // Test 2: Zero matrix
    std::cout << "\n2. Zero matrix:" << '\n';
    auto zero = Matrix4::zero();
    std::cout << mat4_str(zero) << '\n';

    // Test 3: Diagonal matrix
    std::cout << "\n3. Diagonal matrix (3.0f):" << '\n';
    auto diag = Matrix4(3.0f);
    std::cout << mat4_str(diag) << '\n';

    // Test 4: Identity * Vec4
    std::cout << "\n4. Identity * Vec4:" << '\n';
    Vector4 v4{1.0f, 2.0f, 3.0f, 1.0f};
    auto result4 = identity * v4;
    std::cout << "v4 = " << vec4_str(v4) << '\n';
    std::cout << "result = " << vec4_str(result4) << '\n';

    // Test 5: Identity * Vec3
    std::cout << "\n5. Identity * Vec3:" << '\n';
    Vector3 v3{1.0f, 2.0f, 3.0f};
    auto result3 = identity * v3;
    std::cout << "v3 = " << vec3_str(v3) << '\n';
    std::cout << "result = " << vec3_str(result3) << '\n';

    // Test 6: Determinant of identity
    std::cout << "\n6. Determinant test:" << '\n';
    std::cout << "det(identity) = " << Matrix4::determinant(identity) << " (expected: 1)" << '\n';

    // Test 7: Translation matrix
    std::cout << "\n7. Translation (2, 3, 4):" << '\n';
    auto translate = Matrix4::translate({2.0f, 3.0f, 4.0f});
    std::cout << mat4_str(translate) << '\n';

    // Test 8: Transform point with translation
    std::cout << "\n8. Transform point:" << '\n';
    auto point = Matrix4::transform_point(translate, {1.0f, 1.0f, 1.0f});
    std::cout << "translate * (1,1,1) = " << vec3_str(point) << " (expected: (3,4,5))" << '\n';

    // Test 9: Transform vector (no translation)
    std::cout << "\n9. Transform vector:" << '\n';
    auto vec = Matrix4::transform_vector(translate, {1.0f, 0.0f, 0.0f});
    std::cout << "translate * (1,0,0) = " << vec3_str(vec) << " (expected: (1,0,0))" << '\n';

    // Test 10: Scale matrix
    std::cout << "\n10. Scale (2, 3, 4):" << '\n';
    auto scale = Matrix4::scale({2.0f, 3.0f, 4.0f});
    std::cout << mat4_str(scale) << '\n';

    // Test 11: Scale transform
    std::cout << "\n11. Scale transform:" << '\n';
    auto scaled_point = Matrix4::transform_point(scale, {2.0f, 2.0f, 2.0f});
    std::cout << "scale * (2,2,2) = " << vec3_str(scaled_point) << " (expected: (4,6,8))" << '\n';

    // Test 12: Rotation X (90 degrees)
    std::cout << "\n12. Rotation X (90 degrees):" << '\n';
    auto rot_x = Matrix4::rotate_x(gkit::math::PI_32 / 2.0f);
    std::cout << mat4_str(rot_x) << '\n';

    // Verify rotation
    std::cout << "rot_x * (0,1,0) = " << vec3_str(Matrix4::transform_point(rot_x, {0.0f, 1.0f, 0.0f}))
              << " (expected: ~ (0,0,1))" << '\n';

    // Test 13: Rotation Y
    std::cout << "\n13. Rotation Y (90 degrees):" << '\n';
    auto rot_y = Matrix4::rotate_y(gkit::math::PI_32 / 2.0f);
    std::cout << mat4_str(rot_y) << '\n';
    std::cout << "rot_y * (1,0,0) = " << vec3_str(Matrix4::transform_point(rot_y, {1.0f, 0.0f, 0.0f}))
              << " (expected: ~ (0,0,-1))" << '\n';

    // Test 14: Rotation Z
    std::cout << "\n14. Rotation Z (90 degrees):" << '\n';
    auto rot_z = Matrix4::rotate_z(gkit::math::PI_32 / 2.0f);
    std::cout << mat4_str(rot_z) << '\n';
    std::cout << "rot_z * (1,0,0) = " << vec3_str(Matrix4::transform_point(rot_z, {1.0f, 0.0f, 0.0f}))
              << " (expected: ~ (0,1,0))" << '\n';

    // Test 15: Arbitrary axis rotation
    std::cout << "\n15. Rotation around (1,1,1) axis 90 degrees:" << '\n';
    auto rot_axis = Matrix4::rotate(gkit::math::PI_32 / 2.0f, Vector3{1.0f, 1.0f, 1.0f}.normalize());
    std::cout << mat4_str(rot_axis) << '\n';

    // Test 16: Transpose
    std::cout << "\n16. Transpose test:" << '\n';
    auto transposed = Matrix4::transpose(scale);
    std::cout << "transpose(scale):" << '\n';
    std::cout << mat4_str(transposed) << '\n';

    // Test 17: Matrix multiplication
    std::cout << "\n17. Matrix multiplication (T * R * S):" << '\n';
    auto trs = translate * rot_x * scale;
    std::cout << mat4_str(trs) << '\n';

    // Test 18: Inverse of identity
    std::cout << "\n18. Inverse test:" << '\n';
    auto inv_identity = Matrix4::inverse(identity);
    if (inv_identity.has_value()) {
        std::cout << "inverse(identity) exists" << '\n';
    }

    // Test 19: Inverse of TRS
    std::cout << "\n19. Inverse of TRS matrix:" << '\n';
    auto inv_trs = Matrix4::inverse(trs);
    if (inv_trs.has_value()) {
        std::cout << "inverse(TRS) exists" << '\n';
        auto composed = trs * inv_trs.value();
        std::cout << "TRS * inverse(TRS):" << '\n';
        std::cout << mat4_str(composed) << '\n';
    } else {
        std::cout << "TRS is singular (no inverse)" << '\n';
    }

    // Test 20: Get/Set identity
    std::cout << "\n20. Set identity:" << '\n';
    auto mat = Matrix4::zero();
    mat.set_identity();
    std::cout << "After set_identity():" << '\n';
    std::cout << mat4_str(mat) << '\n';

    // Test 21: Extract translation
    std::cout << "\n21. Extract translation from TRS:" << '\n';
    auto extracted_trans = Matrix4::get_translation(trs);
    std::cout << "Translation = " << vec3_str(extracted_trans) << " (expected: (2,3,4))" << '\n';

    // Test 22: Extract rotation
    std::cout << "\n22. Extract rotation from TRS:" << '\n';
    auto extracted_rot = Matrix4::get_rotation(trs);
    std::cout << "Rotation:" << '\n';
    std::cout << mat3_str(extracted_rot) << '\n';

    // Test 23: Extract scale
    std::cout << "\n23. Extract scale from TRS:" << '\n';
    auto extracted_scale = Matrix4::get_scale(trs);
    std::cout << "Scale = " << vec3_str(extracted_scale) << '\n';

    // Test 24: Decompose
    std::cout << "\n24. Decompose TRS:" << '\n';
    auto [t, r, s] = Matrix4::decompose(trs);
    std::cout << "Translation = " << vec3_str(t) << '\n';
    std::cout << "Scale = " << vec3_str(s) << '\n';

    // Test 25: Compose TRS
    std::cout << "\n25. Compose TRS:" << '\n';
    auto composed_trs = Matrix4::compose(extracted_trans, extracted_rot, extracted_scale);
    std::cout << mat4_str(composed_trs) << '\n';

    // Test 26: Perspective projection
    std::cout << "\n26. Perspective projection (fov=60deg, aspect=1.778, near=0.1, far=100):" << '\n';
    auto persp = Matrix4::perspective(gkit::math::PI_32 / 3.0f, 16.0f / 9.0f, 0.1f, 100.0f);
    std::cout << mat4_str(persp) << '\n';

    // Test 27: Orthographic projection
    std::cout << "\n27. Orthographic projection (left=-10, right=10, bottom=-10, top=10, near=0.1, far=100):" << '\n';
    auto ortho = Matrix4::orthographic(-10.0f, 10.0f, -10.0f, 10.0f, 0.1f, 100.0f);
    std::cout << mat4_str(ortho) << '\n';

    // Test 28: Look-at
    std::cout << "\n28. Look-at (eye=(0,0,5), target=(0,0,0), up=(0,1,0)):" << '\n';
    auto look = Matrix4::look_at({0.0f, 0.0f, 5.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f});
    std::cout << mat4_str(look) << '\n';

    // Test 29: Is affine
    std::cout << "\n29. Is affine check:" << '\n';
    std::cout << "identity.is_affine() = " << (identity.is_affine() ? "true" : "false") << " (expected: true)" << '\n';
    std::cout << "persp.is_affine() = " << (persp.is_affine() ? "true" : "false") << " (expected: false)" << '\n';

    // Test 30: Is orthonormal
    std::cout << "\n30. Is orthonormal check:" << '\n';
    std::cout << "rot_x.is_orthonormal() = " << (rot_x.is_orthonormal() ? "true" : "false") << " (expected: true)"
              << '\n';

    // Test 31: Trace
    std::cout << "\n31. Trace:" << '\n';
    std::cout << "trace(identity) = " << identity.trace() << " (expected: 4)" << '\n';

    // Test 32: From rows/columns
    std::cout << "\n32. From rows:" << '\n';
    auto from_rows = Matrix4::from_rows(
        {1.0f, 0.0f, 0.0f, 0.0f}, {0.0f, 2.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 3.0f, 0.0f}, {4.0f, 5.0f, 6.0f, 1.0f});
    std::cout << mat4_str(from_rows) << '\n';

    // Test 33: Lerp
    std::cout << "\n33. Lerp:" << '\n';
    auto lerp_result = Matrix4::lerp(Matrix4::zero(), Matrix4::identity(), 0.5f);
    std::cout << "lerp(zero, identity, 0.5):" << '\n';
    std::cout << mat4_str(lerp_result) << '\n';

    // Test 34: Shear
    std::cout << "\n34. Shear (1, 2, 3):" << '\n';
    auto shear = Matrix4::shear({1.0f, 2.0f, 3.0f});
    std::cout << mat4_str(shear) << '\n';

    // Test 35: Row/Column access
    std::cout << "\n35. Row/Column access:" << '\n';
    auto row0 = identity.row(0);
    auto col0 = identity.column(0);
    std::cout << "identity.row(0) = " << vec4_str(row0) << " (expected: (1,0,0,0))" << '\n';
    std::cout << "identity.column(0) = " << vec4_str(col0) << " (expected: (1,0,0,0))" << '\n';

    // Test 36: Data pointer
    std::cout << "\n36. Data pointer:" << '\n';
    const float* data_ptr = identity.data();
    std::cout << "identity.data()[0] = " << data_ptr[0] << " (expected: 1)" << '\n';
    std::cout << "identity.data()[5] = " << data_ptr[5] << " (expected: 1, diagonal element)" << '\n';

    // Test 37: To string
    std::cout << "\n37. To string:" << '\n';
    std::cout << identity.to_string() << '\n';

    // Test 38: Compound operators
    std::cout << "\n38. Compound operators:" << '\n';
    auto compound = Matrix4::identity();
    compound += Matrix4(1.0f);
    std::cout << "identity += Matrix4(1):" << '\n';
    std::cout << mat4_str(compound) << '\n';

    compound *= 2.0f;
    std::cout << "after *= 2:" << '\n';
    std::cout << mat4_str(compound) << '\n';

    // Test 39: Rotation between vectors
    std::cout << "\n39. Rotation between vectors:" << '\n';
    auto rot_between = Matrix4::rotation_between_vectors({1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f});
    std::cout << mat4_str(rot_between) << '\n';
    std::cout << "rot_between * (1,0,0) = " << vec3_str(Matrix4::transform_vector(rot_between, {1.0f, 0.0f, 0.0f}))
              << '\n';

    // Test 40: Get quaternion
    std::cout << "\n40. Get quaternion from rotation:" << '\n';
    auto quat = Matrix4::get_quaternion(rot_x);
    std::cout << "Quaternion from rot_x(90deg): " << vec4_str(quat) << '\n';

    std::cout << "\n=== All tests completed! ===" << '\n';
    return 0;
}