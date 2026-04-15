#include <cstdio>
#include <cassert>
#include "gkit/math/constants.hpp"
#include "gkit/math/scalar.hpp"

using namespace gkit::math;

int main() {
    // ====== test scalar.hpp ======

    // Type alias tests
    static_assert(sizeof(gkit::math::int8)   == 1, "int8 size error");
    static_assert(sizeof(gkit::math::int16)  == 2, "int16 size error");
    static_assert(sizeof(gkit::math::int32)  == 4, "int32 size error");
    static_assert(sizeof(gkit::math::int64)  == 8, "int64 size error");
    static_assert(sizeof(gkit::math::uint8)  == 1, "uint8 size error");
    static_assert(sizeof(gkit::math::uint16) == 2, "uint16 size error");
    static_assert(sizeof(gkit::math::uint32) == 4, "uint32 size error");
    static_assert(sizeof(gkit::math::uint64) == 8, "uint64 size error");
    static_assert(sizeof(gkit::math::float32) == 4, "float32 size error");
    static_assert(sizeof(gkit::math::float64) == 8, "float64 size error");

    // ScalarLimits tests
    assert(ScalarLimits<int32>::min_v    == -2147483647 - 1);
    assert(ScalarLimits<int32>::max_v    == 2147483647);
    assert(ScalarLimits<float32>::epsilon_v > 0);

    // fp constants tests
    assert(fp::EPSILON32 > 0);
    assert(fp::MIN32 > 0);
    assert(fp::MAX32 > fp::MIN32);

    // integer constants tests
    assert(int_limits::I8_MIN  == -128);
    assert(int_limits::I8_MAX  == 127);
    assert(int_limits::U8_MAX == 255);
    assert(int_limits::I16_MIN == -32768);
    assert(int_limits::I16_MAX == 32767);
    assert(int_limits::U16_MAX == 65535);

    // ====== Test constants.hpp ======

    // Math constants tests
    assert(gkit::math::math_const::PI_32 > 3.14f && gkit::math::math_const::PI_32 < 3.15f);
    assert(gkit::math::math_const::TWO_PI_32 > 6.28f && gkit::math::math_const::TWO_PI_32 < 6.29f);
    assert(gkit::math::math_const::HALF_PI_32 > 1.57f && gkit::math::math_const::HALF_PI_32 < 1.58f);
    assert(gkit::math::math_const::INV_PI_32 > 0.31f && gkit::math::math_const::INV_PI_32 < 0.32f);
    assert(gkit::math::math_const::E_32 > 2.71f && gkit::math::math_const::E_32 < 2.72f);
    assert(gkit::math::math_const::PHI_32 > 1.61f && gkit::math::math_const::PHI_32 < 1.62f);
    assert(gkit::math::math_const::SQRT_2_32 > 1.41f && gkit::math::math_const::SQRT_2_32 < 1.42f);
    assert(gkit::math::math_const::LN_2_32 > 0.69f && gkit::math::math_const::LN_2_32 < 0.70f);

    // Angle conversion tests
    assert(gkit::math::angle_const::DEG_TO_RAD_32 > 0.017f && gkit::math::angle_const::DEG_TO_RAD_32 < 0.018f);
    assert(gkit::math::angle_const::RAD_TO_DEG_32 > 57.2f && gkit::math::angle_const::RAD_TO_DEG_32 < 57.3f);

    // Numeric constants tests
    assert(gkit::math::numeric_const::ZERO_32 == 0.0f);
    assert(gkit::math::numeric_const::ONE_32 == 1.0f);
    assert(gkit::math::numeric_const::NEG_ONE_32 == -1.0f);
    assert(gkit::math::numeric_const::TWO_32 == 2.0f);
    assert(gkit::math::numeric_const::HALF_32 == 0.5f);
    assert(gkit::math::numeric_const::QUARTER_32 == 0.25f);

    assert(gkit::math::numeric_const::ZERO_I32 == 0);
    assert(gkit::math::numeric_const::ONE_I32 == 1);
    assert(gkit::math::numeric_const::NEG_ONE_I32 == -1);
    assert(gkit::math::numeric_const::TWO_I32 == 2);

    // Print test output
    printf("=== scalar.hpp tests ===\n");
    printf("int32 min: %d, max: %d\n",
           gkit::math::ScalarLimits<int32>::min_v,
           gkit::math::ScalarLimits<int32>::max_v);
    printf("float32 epsilon: %.10f\n", gkit::math::ScalarLimits<float32>::epsilon_v);

    printf("\n=== constants.hpp tests ===\n");
    printf("PI_32: %.10f\n", gkit::math::math_const::PI_32);
    printf("TWO_PI_32: %.10f\n", gkit::math::math_const::TWO_PI_32);
    printf("E_32: %.10f\n", gkit::math::math_const::E_32);
    printf("PHI_32: %.10f\n", gkit::math::math_const::PHI_32);
    printf("SQRT_2_32: %.10f\n", gkit::math::math_const::SQRT_2_32);
    printf("DEG_TO_RAD_32: %.10f\n", gkit::math::angle_const::DEG_TO_RAD_32);
    printf("RAD_TO_DEG_32: %.10f\n", gkit::math::angle_const::RAD_TO_DEG_32);

    printf("\nAll tests passed!\n");
    return 0;
}