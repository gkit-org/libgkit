#include "gkit/math/constants.hpp"
#include "gkit/math/scalar.hpp"
#include "test_utils.hpp"

auto test_scalar_limits() -> bool {
    gkit::test::logln("=== scalar.hpp tests ===");

    using namespace gkit::math; // NOLINT(google-build-using-namespace

    gkit::test::assert_if(ScalarLimits<int32_t>::MIN_V == -2147483647 - 1, "ScalarLimits<int32_t>::MIN_V is wrong");
    gkit::test::assert_if(ScalarLimits<int32_t>::MAX_V == 2147483647, "ScalarLimits<int32_t>::MAX_V is wrong");
    gkit::test::assert_if(ScalarLimits<float>::EPSILON_V > 0, "ScalarLimits<float>::EPSILON_V must be positive");

    gkit::test::logln("  int32 min: {}, max: {}", ScalarLimits<int32_t>::MIN_V, ScalarLimits<int32_t>::MAX_V);
    gkit::test::logln("  float32 epsilon: {:.10f}", ScalarLimits<float>::EPSILON_V);
    return true;
}


auto test_float_constants() -> bool {
    gkit::test::logln("=== constants.hpp float constants tests ===");

    using namespace gkit::math; // NOLINT(google-build-using-namespace

    gkit::test::assert_if(EPSILON32 > 0, "EPSILON32 must be positive");
    gkit::test::assert_if(MIN32 > 0, "MIN32 must be positive");
    gkit::test::assert_if(MAX32 > MIN32, "MAX32 must be greater than MIN32");

    gkit::test::assert_if(PI_32 > 3.14f && PI_32 < 3.15f, "PI_32 out of expected range");
    gkit::test::assert_if(TWO_PI_32 > 6.28f && TWO_PI_32 < 6.29f, "TWO_PI_32 out of expected range");
    gkit::test::assert_if(HALF_PI_32 > 1.57f && HALF_PI_32 < 1.58f, "HALF_PI_32 out of expected range");
    gkit::test::assert_if(INV_PI_32 > 0.31f && INV_PI_32 < 0.32f, "INV_PI_32 out of expected range");
    gkit::test::assert_if(E_32 > 2.71f && E_32 < 2.72f, "E_32 out of expected range");
    gkit::test::assert_if(PHI_32 > 1.61f && PHI_32 < 1.62f, "PHI_32 out of expected range");
    gkit::test::assert_if(SQRT_2_32 > 1.41f && SQRT_2_32 < 1.42f, "SQRT_2_32 out of expected range");
    gkit::test::assert_if(LN_2_32 > 0.69f && LN_2_32 < 0.70f, "LN_2_32 out of expected range");

    gkit::test::assert_if(DEG_TO_RAD_32 > 0.017f && DEG_TO_RAD_32 < 0.018f, "DEG_TO_RAD_32 out of expected range");
    gkit::test::assert_if(RAD_TO_DEG_32 > 57.2f && RAD_TO_DEG_32 < 57.3f, "RAD_TO_DEG_32 out of expected range");

    gkit::test::logln("  PI_32: {:.10f}", PI_32);
    gkit::test::logln("  TWO_PI_32: {:.10f}", TWO_PI_32);
    gkit::test::logln("  E_32: {:.10f}", E_32);
    gkit::test::logln("  PHI_32: {:.10f}", PHI_32);
    gkit::test::logln("  SQRT_2_32: {:.10f}", SQRT_2_32);
    gkit::test::logln("  DEG_TO_RAD_32: {:.10f}", DEG_TO_RAD_32);
    gkit::test::logln("  RAD_TO_DEG_32: {:.10f}", RAD_TO_DEG_32);
    return true;
}


auto test_integer_constants() -> bool {
    gkit::test::logln("=== constants.hpp integer constants tests ===");

    using namespace gkit::math; // NOLINT(google-build-using-namespace

    gkit::test::assert_if(I8_MIN == -128, "I8_MIN is wrong");
    gkit::test::assert_if(I8_MAX == 127, "I8_MAX is wrong");
    gkit::test::assert_if(U8_MAX == 255, "U8_MAX is wrong");
    gkit::test::assert_if(I16_MIN == -32768, "I16_MIN is wrong");
    gkit::test::assert_if(I16_MAX == 32767, "I16_MAX is wrong");
    gkit::test::assert_if(U16_MAX == 65535, "U16_MAX is wrong");

    gkit::test::assert_if(ZERO_32 == 0.0f, "ZERO_32 is wrong");
    gkit::test::assert_if(ONE_32 == 1.0f, "ONE_32 is wrong");
    gkit::test::assert_if(NEG_ONE_32 == -1.0f, "NEG_ONE_32 is wrong");
    gkit::test::assert_if(TWO_32 == 2.0f, "TWO_32 is wrong");
    gkit::test::assert_if(HALF_32 == 0.5f, "HALF_32 is wrong");
    gkit::test::assert_if(QUARTER_32 == 0.25f, "QUARTER_32 is wrong");

    gkit::test::assert_if(ZERO_I32 == 0, "ZERO_I32 is wrong");
    gkit::test::assert_if(ONE_I32 == 1, "ONE_I32 is wrong");
    gkit::test::assert_if(NEG_ONE_I32 == -1, "NEG_ONE_I32 is wrong");
    gkit::test::assert_if(TWO_I32 == 2, "TWO_I32 is wrong");
    return true;
}


auto main() -> int {
    auto test_runner = gkit::test::TestRunner()
                           .add_test_func(test_scalar_limits)
                           .add_test_func(test_float_constants)
                           .add_test_func(test_integer_constants);

    test_runner.run();
    return 0;
}
