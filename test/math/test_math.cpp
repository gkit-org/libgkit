#include <cstdio>
#include <cassert>
#include "gkit/math/constants.hpp"
#include "gkit/math/scalar.hpp"

using namespace gkit::math;

int main() {
    // ====== test scalar.hpp ======

    // ScalarLimits tests
    assert(ScalarLimits<int32_t>::min_v    == -2147483647 - 1);
    assert(ScalarLimits<int32_t>::max_v    == 2147483647);
    assert(ScalarLimits<float>::epsilon_v > 0);

    // fp constants tests
    assert(EPSILON32 > 0);
    assert(MIN32 > 0);
    assert(MAX32 > MIN32);

    // integer constants tests
    assert(I8_MIN  == -128);
    assert(I8_MAX  == 127);
    assert(U8_MAX == 255);
    assert(I16_MIN == -32768);
    assert(I16_MAX == 32767);
    assert(U16_MAX == 65535);

    // ====== Test constants.hpp ======

    // Math constants tests
    assert(gkit::math::PI_32 > 3.14f && gkit::math::PI_32 < 3.15f);
    assert(gkit::math::TWO_PI_32 > 6.28f && gkit::math::TWO_PI_32 < 6.29f);
    assert(gkit::math::HALF_PI_32 > 1.57f && gkit::math::HALF_PI_32 < 1.58f);
    assert(gkit::math::INV_PI_32 > 0.31f && gkit::math::INV_PI_32 < 0.32f);
    assert(gkit::math::E_32 > 2.71f && gkit::math::E_32 < 2.72f);
    assert(gkit::math::PHI_32 > 1.61f && gkit::math::PHI_32 < 1.62f);
    assert(gkit::math::SQRT_2_32 > 1.41f && gkit::math::SQRT_2_32 < 1.42f);
    assert(gkit::math::LN_2_32 > 0.69f && gkit::math::LN_2_32 < 0.70f);

    // Angle conversion tests
    assert(gkit::math::DEG_TO_RAD_32 > 0.017f && gkit::math::DEG_TO_RAD_32 < 0.018f);
    assert(gkit::math::RAD_TO_DEG_32 > 57.2f && gkit::math::RAD_TO_DEG_32 < 57.3f);

    // Numeric constants tests
    assert(gkit::math::ZERO_32 == 0.0f);
    assert(gkit::math::ONE_32 == 1.0f);
    assert(gkit::math::NEG_ONE_32 == -1.0f);
    assert(gkit::math::TWO_32 == 2.0f);
    assert(gkit::math::HALF_32 == 0.5f);
    assert(gkit::math::QUARTER_32 == 0.25f);

    assert(gkit::math::ZERO_I32 == 0);
    assert(gkit::math::ONE_I32 == 1);
    assert(gkit::math::NEG_ONE_I32 == -1);
    assert(gkit::math::TWO_I32 == 2);

    // Print test output
    printf("=== scalar.hpp tests ===\n");
    printf("int32 min: %d, max: %d\n",
           gkit::math::ScalarLimits<int32_t>::min_v,
           gkit::math::ScalarLimits<int32_t>::max_v);
    printf("float32 epsilon: %.10f\n", gkit::math::ScalarLimits<float>::epsilon_v);

    printf("\n=== constants.hpp tests ===\n");
    printf("PI_32: %.10f\n", gkit::math::PI_32);
    printf("TWO_PI_32: %.10f\n", gkit::math::TWO_PI_32);
    printf("E_32: %.10f\n", gkit::math::E_32);
    printf("PHI_32: %.10f\n", gkit::math::PHI_32);
    printf("SQRT_2_32: %.10f\n", gkit::math::SQRT_2_32);
    printf("DEG_TO_RAD_32: %.10f\n", gkit::math::DEG_TO_RAD_32);
    printf("RAD_TO_DEG_32: %.10f\n", gkit::math::RAD_TO_DEG_32);

    printf("\nAll tests passed!\n");
    return 0;
}