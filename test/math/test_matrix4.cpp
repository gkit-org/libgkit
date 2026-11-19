#include "test_utils.hpp"

#include <format>
#include <iomanip>
#include <sstream>
#include <string>

#include <gkit/math/constants.hpp>
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


auto test_matrix_constructors() -> bool {
    using namespace gkit::math; // NOLINT(google-build-using-namespace
    gkit::test::logln("=== matrix constructors ===");

    auto identity = Matrix4::identity();
    gkit::test::logln("  identity:\n{}", mat4_str(identity));

    auto zero = Matrix4::zero();
    gkit::test::logln("  zero:\n{}", mat4_str(zero));

    auto diag = Matrix4(3.0f);
    gkit::test::logln("  diagonal (3.0f):\n{}", mat4_str(diag));
    gkit::test::assert_if(diag.m[0][0] == 3.0f && diag.m[1][1] == 3.0f && diag.m[2][2] == 3.0f && diag.m[3][3] == 3.0f,
                          "diagonal constructor failed");
    return true;
}


auto test_matrix_vector_multiplication() -> bool {
    using namespace gkit::math; // NOLINT(google-build-using-namespace
    gkit::test::logln("=== matrix * vector ===");

    auto identity = Matrix4::identity();

    Vector4 v4{1.0f, 2.0f, 3.0f, 1.0f};
    auto result4 = identity * v4;
    gkit::test::assert_if(result4.x == 1.0f && result4.y == 2.0f && result4.z == 3.0f && result4.w == 1.0f,
                          "identity * vec4 failed");
    gkit::test::logln("  v4 = {}, result = {}", vec4_str(v4), vec4_str(result4));

    Vector3 v3{1.0f, 2.0f, 3.0f};
    auto result3 = identity * v3;
    gkit::test::assert_if(result3.x == 1.0f && result3.y == 2.0f && result3.z == 3.0f, "identity * vec3 failed");
    gkit::test::logln("  v3 = {}, result = {}", vec3_str(v3), vec3_str(result3));

    auto translate = Matrix4::translate({2.0f, 3.0f, 4.0f});
    auto point     = Matrix4::transform_point(translate, {1.0f, 1.0f, 1.0f});
    gkit::test::assert_if(point.x == 3.0f && point.y == 4.0f && point.z == 5.0f, "translate * point failed");
    gkit::test::logln("  translate * (1,1,1) = {} (expected: (3,4,5))", vec3_str(point));

    auto vec = Matrix4::transform_vector(translate, {1.0f, 0.0f, 0.0f});
    gkit::test::assert_if(vec.x == 1.0f && vec.y == 0.0f && vec.z == 0.0f, "transform_vector must not translate");
    gkit::test::logln("  translate * (1,0,0) = {} (expected: (1,0,0))", vec3_str(vec));
    return true;
}


auto test_determinant() -> bool {
    using namespace gkit::math; // NOLINT(google-build-using-namespace
    gkit::test::logln("=== determinant ===");

    auto identity = Matrix4::identity();
    gkit::test::assert_if(Matrix4::determinant(identity) == 1.0f, "det(identity) must be 1");
    gkit::test::logln("  det(identity) = {} (expected: 1)", Matrix4::determinant(identity));
    return true;
}


auto test_translation_and_scale() -> bool {
    using namespace gkit::math; // NOLINT(google-build-using-namespace
    gkit::test::logln("=== translation and scale ===");

    auto translate = Matrix4::translate({2.0f, 3.0f, 4.0f});
    gkit::test::logln("  translation (2, 3, 4):\n{}", mat4_str(translate));

    auto scale = Matrix4::scale({2.0f, 3.0f, 4.0f});
    gkit::test::logln("  scale (2, 3, 4):\n{}", mat4_str(scale));

    auto scaled_point = Matrix4::transform_point(scale, {2.0f, 2.0f, 2.0f});
    gkit::test::assert_if(scaled_point.x == 4.0f && scaled_point.y == 6.0f && scaled_point.z == 8.0f,
                          "scale * point failed");
    gkit::test::logln("  scale * (2,2,2) = {} (expected: (4,6,8))", vec3_str(scaled_point));
    return true;
}


auto test_rotation() -> bool {
    using namespace gkit::math; // NOLINT(google-build-using-namespace
    gkit::test::logln("=== rotation ===");

    auto rot_x = Matrix4::rotate_x(gkit::math::PI_32 / 2.0f);
    gkit::test::logln("  rot_x (90 degrees):\n{}", mat4_str(rot_x));
    auto px = Matrix4::transform_point(rot_x, {0.0f, 1.0f, 0.0f});
    gkit::test::assert_if(std::abs(px.x) < 1e-4f && std::abs(px.y) < 1e-4f && std::abs(px.z - 1.0f) < 1e-4f,
                          "rot_x * (0,1,0) must be ~(0,0,1)");
    gkit::test::logln("  rot_x * (0,1,0) = {} (expected: ~ (0,0,1))", vec3_str(px));

    auto rot_y = Matrix4::rotate_y(gkit::math::PI_32 / 2.0f);
    gkit::test::logln("  rot_y (90 degrees):\n{}", mat4_str(rot_y));
    auto py = Matrix4::transform_point(rot_y, {1.0f, 0.0f, 0.0f});
    gkit::test::assert_if(std::abs(py.x) < 1e-4f && std::abs(py.y) < 1e-4f && std::abs(py.z + 1.0f) < 1e-4f,
                          "rot_y * (1,0,0) must be ~(0,0,-1)");
    gkit::test::logln("  rot_y * (1,0,0) = {} (expected: ~ (0,0,-1))", vec3_str(py));

    auto rot_z = Matrix4::rotate_z(gkit::math::PI_32 / 2.0f);
    gkit::test::logln("  rot_z (90 degrees):\n{}", mat4_str(rot_z));
    auto pz = Matrix4::transform_point(rot_z, {1.0f, 0.0f, 0.0f});
    gkit::test::assert_if(std::abs(pz.x) < 1e-4f && std::abs(pz.y - 1.0f) < 1e-4f && std::abs(pz.z) < 1e-4f,
                          "rot_z * (1,0,0) must be ~(0,1,0)");
    gkit::test::logln("  rot_z * (1,0,0) = {} (expected: ~ (0,1,0))", vec3_str(pz));
    return true;
}


auto test_axis_rotation() -> bool {
    using namespace gkit::math; // NOLINT(google-build-using-namespace
    gkit::test::logln("=== rotation around arbitrary axis ===");

    auto rot_axis = Matrix4::rotate(gkit::math::PI_32 / 2.0f, Vector3{1.0f, 1.0f, 1.0f}.normalize());
    gkit::test::logln("  rotation around (1,1,1) axis 90 degrees:\n{}", mat4_str(rot_axis));
    gkit::test::assert_if(Matrix4::determinant(rot_axis) > 0.99f && Matrix4::determinant(rot_axis) < 1.01f,
                          "axis rotation determinant must be 1");
    return true;
}


auto test_transpose_and_multiplication() -> bool {
    using namespace gkit::math; // NOLINT(google-build-using-namespace
    gkit::test::logln("=== transpose and multiplication ===");

    auto scale      = Matrix4::scale({2.0f, 3.0f, 4.0f});
    auto transposed = Matrix4::transpose(scale);
    gkit::test::logln("  transpose(scale):\n{}", mat4_str(transposed));
    gkit::test::assert_if(std::abs(transposed.m[0][0] - 2.0f) < 1e-4f && std::abs(transposed.m[1][1] - 3.0f) < 1e-4f &&
                              std::abs(transposed.m[2][2] - 4.0f) < 1e-4f,
                          "transpose of diagonal must be itself");

    auto translate = Matrix4::translate({2.0f, 3.0f, 4.0f});
    auto rot_x     = Matrix4::rotate_x(gkit::math::PI_32 / 2.0f);
    auto trs       = translate * rot_x * scale;
    gkit::test::logln("  TRS (T * R * S):\n{}", mat4_str(trs));
    return true;
}


auto test_inverse() -> bool {
    using namespace gkit::math; // NOLINT(google-build-using-namespace
    gkit::test::logln("=== inverse ===");

    auto identity     = Matrix4::identity();
    auto inv_identity = Matrix4::inverse(identity);
    gkit::test::assert_if(inv_identity.has_value(), "inverse(identity) must have a value");
    gkit::test::logln("  inverse(identity) exists");

    auto translate = Matrix4::translate({2.0f, 3.0f, 4.0f});
    auto rot_x     = Matrix4::rotate_x(gkit::math::PI_32 / 2.0f);
    auto scale     = Matrix4::scale({2.0f, 3.0f, 4.0f});
    auto trs       = translate * rot_x * scale;

    auto inv_trs = Matrix4::inverse(trs);
    gkit::test::assert_if(inv_trs.has_value(), "inverse(TRS) must have a value");
    auto composed = trs * inv_trs.value();
    gkit::test::logln("  TRS * inverse(TRS):\n{}", mat4_str(composed));
    gkit::test::assert_if(std::abs(composed.m[0][0] - 1.0f) < 1e-4f && std::abs(composed.m[1][1] - 1.0f) < 1e-4f &&
                              std::abs(composed.m[2][2] - 1.0f) < 1e-4f && std::abs(composed.m[3][3] - 1.0f) < 1e-4f,
                          "TRS * inverse(TRS) must be identity");
    return true;
}


auto test_set_identity_and_extract() -> bool {
    using namespace gkit::math; // NOLINT(google-build-using-namespace
    gkit::test::logln("=== set_identity and extract ===");

    auto mat = Matrix4::zero();
    mat.set_identity();
    gkit::test::logln("  after set_identity():\n{}", mat4_str(mat));
    gkit::test::assert_if(mat.m[0][0] == 1.0f && mat.m[1][1] == 1.0f && mat.m[2][2] == 1.0f && mat.m[3][3] == 1.0f,
                          "set_identity failed");

    auto translate = Matrix4::translate({2.0f, 3.0f, 4.0f});
    auto rot_x     = Matrix4::rotate_x(gkit::math::PI_32 / 2.0f);
    auto scale     = Matrix4::scale({2.0f, 3.0f, 4.0f});
    auto trs       = translate * rot_x * scale;

    auto extracted_trans = Matrix4::get_translation(trs);
    gkit::test::assert_if(extracted_trans.x == 2.0f && extracted_trans.y == 3.0f && extracted_trans.z == 4.0f,
                          "get_translation failed");
    gkit::test::logln("  translation = {} (expected: (2,3,4))", vec3_str(extracted_trans));

    auto extracted_rot = Matrix4::get_rotation(trs);
    gkit::test::logln("  rotation:\n{}", mat3_str(extracted_rot));

    auto extracted_scale = Matrix4::get_scale(trs);
    gkit::test::assert_if(std::abs(extracted_scale.x - 2.0f) < 1e-3f && std::abs(extracted_scale.y - 3.0f) < 1e-3f &&
                              std::abs(extracted_scale.z - 4.0f) < 1e-3f,
                          "get_scale failed");
    gkit::test::logln("  scale = {}", vec3_str(extracted_scale));

    auto [t, r, s] = Matrix4::decompose(trs);
    gkit::test::assert_if(std::abs(t.x - 2.0f) < 1e-3f && std::abs(t.y - 3.0f) < 1e-3f && std::abs(t.z - 4.0f) < 1e-3f,
                          "decompose translation failed");
    gkit::test::assert_if(std::abs(s.x - 2.0f) < 1e-3f && std::abs(s.y - 3.0f) < 1e-3f && std::abs(s.z - 4.0f) < 1e-3f,
                          "decompose scale failed");
    gkit::test::logln("  decompose: translation = {}, scale = {}", vec3_str(t), vec3_str(s));

    auto composed_trs = Matrix4::compose(extracted_trans, extracted_rot, extracted_scale);
    gkit::test::logln("  compose(TRS):\n{}", mat4_str(composed_trs));
    return true;
}


auto test_projection() -> bool {
    using namespace gkit::math; // NOLINT(google-build-using-namespace
    gkit::test::logln("=== projection ===");

    auto persp = Matrix4::perspective(gkit::math::PI_32 / 3.0f, 16.0f / 9.0f, 0.1f, 100.0f);
    gkit::test::logln("  perspective:\n{}", mat4_str(persp));

    auto ortho = Matrix4::orthographic(-10.0f, 10.0f, -10.0f, 10.0f, 0.1f, 100.0f);
    gkit::test::logln("  orthographic:\n{}", mat4_str(ortho));

    auto look = Matrix4::look_at({0.0f, 0.0f, 5.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f});
    gkit::test::logln("  look_at:\n{}", mat4_str(look));
    return true;
}


auto test_matrix_properties() -> bool {
    using namespace gkit::math; // NOLINT(google-build-using-namespace
    gkit::test::logln("=== matrix properties ===");

    auto identity = Matrix4::identity();
    gkit::test::assert_if(identity.is_affine(), "identity must be affine");
    gkit::test::logln("  identity.is_affine() = {} (expected: true)", identity.is_affine());

    auto persp = Matrix4::perspective(gkit::math::PI_32 / 3.0f, 16.0f / 9.0f, 0.1f, 100.0f);
    gkit::test::assert_if(!persp.is_affine(), "perspective must not be affine");
    gkit::test::logln("  persp.is_affine() = {} (expected: false)", persp.is_affine());

    auto rot_x = Matrix4::rotate_x(gkit::math::PI_32 / 2.0f);
    gkit::test::assert_if(rot_x.is_orthonormal(), "rot_x must be orthonormal");
    gkit::test::logln("  rot_x.is_orthonormal() = {} (expected: true)", rot_x.is_orthonormal());

    gkit::test::assert_if(identity.trace() == 4.0f, "trace(identity) must be 4");
    gkit::test::logln("  trace(identity) = {} (expected: 4)", identity.trace());
    return true;
}


auto test_from_rows_lerp_shear() -> bool {
    using namespace gkit::math; // NOLINT(google-build-using-namespace
    gkit::test::logln("=== from_rows, lerp, shear ===");

    auto from_rows = Matrix4::from_rows(
        {1.0f, 0.0f, 0.0f, 0.0f}, {0.0f, 2.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 3.0f, 0.0f}, {4.0f, 5.0f, 6.0f, 1.0f});
    gkit::test::logln("  from_rows:\n{}", mat4_str(from_rows));

    auto lerp_result = Matrix4::lerp(Matrix4::zero(), Matrix4::identity(), 0.5f);
    gkit::test::logln("  lerp(zero, identity, 0.5):\n{}", mat4_str(lerp_result));
    gkit::test::assert_if(std::abs(lerp_result.m[0][0] - 0.5f) < 1e-4f &&
                              std::abs(lerp_result.m[1][1] - 0.5f) < 1e-4f &&
                              std::abs(lerp_result.m[2][2] - 0.5f) < 1e-4f,
                          "lerp(zero, identity, 0.5) failed");

    auto shear = Matrix4::shear({1.0f, 2.0f, 3.0f});
    gkit::test::logln("  shear (1, 2, 3):\n{}", mat4_str(shear));
    return true;
}


auto test_accessors() -> bool {
    using namespace gkit::math; // NOLINT(google-build-using-namespace
    gkit::test::logln("=== accessors and to_string ===");

    auto identity = Matrix4::identity();

    auto row0 = identity.row(0);
    gkit::test::assert_if(row0.x == 1.0f && row0.y == 0.0f && row0.z == 0.0f && row0.w == 0.0f,
                          "identity.row(0) failed");
    gkit::test::logln("  identity.row(0) = {} (expected: (1,0,0,0))", vec4_str(row0));

    auto col0 = identity.column(0);
    gkit::test::assert_if(col0.x == 1.0f && col0.y == 0.0f && col0.z == 0.0f && col0.w == 0.0f,
                          "identity.column(0) failed");
    gkit::test::logln("  identity.column(0) = {} (expected: (1,0,0,0))", vec4_str(col0));

    const float* data_ptr = identity.data();
    gkit::test::assert_if(data_ptr[0] == 1.0f, "identity.data()[0] must be 1");
    gkit::test::assert_if(data_ptr[5] == 1.0f, "identity.data()[5] must be 1 (diagonal element)");
    gkit::test::logln("  identity.data()[0] = {} (expected: 1)", data_ptr[0]);
    gkit::test::logln("  identity.data()[5] = {} (expected: 1)", data_ptr[5]);

    gkit::test::logln("  to_string:\n{}", identity.to_string());
    return true;
}


auto test_compound_operators() -> bool {
    using namespace gkit::math; // NOLINT(google-build-using-namespace
    gkit::test::logln("=== compound operators ===");

    auto compound = Matrix4::identity();
    compound += Matrix4(1.0f);
    gkit::test::logln("  identity += Matrix4(1):\n{}", mat4_str(compound));
    gkit::test::assert_if(compound.m[0][0] == 2.0f && compound.m[1][1] == 2.0f, "operator+= failed");

    compound *= 2.0f;
    gkit::test::logln("  after *= 2:\n{}", mat4_str(compound));
    gkit::test::assert_if(compound.m[0][0] == 4.0f && compound.m[1][1] == 4.0f, "operator*= failed");
    return true;
}


auto test_rotation_between_and_quaternion() -> bool {
    using namespace gkit::math; // NOLINT(google-build-using-namespace
    gkit::test::logln("=== rotation between vectors and quaternion ===");

    auto rot_between = Matrix4::rotation_between_vectors({1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f});
    gkit::test::logln("  rotation_between:\n{}", mat4_str(rot_between));
    auto rotated = Matrix4::transform_vector(rot_between, {1.0f, 0.0f, 0.0f});
    gkit::test::assert_if(std::abs(rotated.x) < 1e-3f && std::abs(rotated.y - 1.0f) < 1e-3f &&
                              std::abs(rotated.z) < 1e-3f,
                          "rotation_between_vectors failed");
    gkit::test::logln("  rot_between * (1,0,0) = {}", vec3_str(rotated));

    auto rot_x = Matrix4::rotate_x(gkit::math::PI_32 / 2.0f);
    auto quat  = Matrix4::get_quaternion(rot_x);
    gkit::test::logln("  quaternion from rot_x(90deg): {}", vec4_str(quat));
    return true;
}


auto main() -> int {
    auto test_runner = gkit::test::TestRunner()
                           .add_test_func(test_matrix_constructors)
                           .add_test_func(test_matrix_vector_multiplication)
                           .add_test_func(test_determinant)
                           .add_test_func(test_translation_and_scale)
                           .add_test_func(test_rotation)
                           .add_test_func(test_axis_rotation)
                           .add_test_func(test_transpose_and_multiplication)
                           .add_test_func(test_inverse)
                           .add_test_func(test_set_identity_and_extract)
                           .add_test_func(test_projection)
                           .add_test_func(test_matrix_properties)
                           .add_test_func(test_from_rows_lerp_shear)
                           .add_test_func(test_accessors)
                           .add_test_func(test_compound_operators)
                           .add_test_func(test_rotation_between_and_quaternion);

    test_runner.run();
    return 0;
}
