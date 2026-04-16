#pragma once

#include <numbers>

namespace gkit::math {
    // Mathematical constants (IEEE 754 standard)
    // Pi multiples
    constexpr float PI_32    = std::numbers::pi_v<float>;
    constexpr float TWO_PI_32 = PI_32 * 2.0f;
    constexpr float HALF_PI_32 = PI_32 * 0.5f;
    constexpr float INV_PI_32 = std::numbers::inv_pi_v<float>;

    constexpr float PI_64    = std::numbers::pi;
    constexpr float TWO_PI_64 = PI_64 * 2.0;
    constexpr float HALF_PI_64 = PI_64 * 0.5;
    constexpr float INV_PI_64 = std::numbers::inv_pi;

    // Natural constants
    constexpr float E_32     = std::numbers::e_v<float>;
    constexpr float E_64     = std::numbers::e;

    // Golden ratio
    constexpr float PHI_32   = std::numbers::phi_v<float>;
    constexpr float PHI_64   = std::numbers::phi;

    // Powers and roots
    constexpr float SQRT_2_32   = std::numbers::sqrt2_v<float>;
    constexpr float SQRT_3_32   = std::numbers::sqrt3_v<float>;
    constexpr float SQRT_5_32   = 2.2360679774997897f;  // sqrt(5)
    constexpr float LN_2_32    = std::numbers::ln2_v<float>;
    constexpr float LN_10_32   = std::numbers::ln10_v<float>;

    constexpr float SQRT_2_64   = std::numbers::sqrt2;
    constexpr float SQRT_3_64   = std::numbers::sqrt3;
    constexpr float SQRT_5_64   = 2.2360679774997896964091736687313;  // sqrt(5)
    constexpr float LN_2_64    = std::numbers::ln2;
    constexpr float LN_10_64   = std::numbers::ln10;

    // Angle conversion constants
    constexpr float DEG_TO_RAD_32 = gkit::math::PI_32 / 180.0f;
    constexpr float RAD_TO_DEG_32 = 180.0f / gkit::math::PI_32;

    constexpr float DEG_TO_RAD_64 = gkit::math::PI_64 / 180.0;
    constexpr float RAD_TO_DEG_64 = 180.0 / gkit::math::PI_64;

    // Common numeric constants
    constexpr float ZERO_32         = 0.0f;
    constexpr float ONE_32          = 1.0f;
    constexpr float NEG_ONE_32      = -1.0f;
    constexpr float TWO_32          = 2.0f;
    constexpr float THREE_32        = 3.0f;
    constexpr float FOUR_32         = 4.0f;
    constexpr float FIVE_32         = 5.0f;
    constexpr float TEN_32          = 10.0f;
    constexpr float HUNDRED_32      = 100.0f;
    constexpr float THOUSAND_32     = 1000.0f;
    constexpr float HALF_32         = 0.5f;
    constexpr float QUARTER_32      = 0.25f;
    constexpr float THIRD_32        = 0.3333333333333333333333333333333f;

    constexpr float ZERO_64         = 0.0;
    constexpr float ONE_64          = 1.0;
    constexpr float NEG_ONE_64      = -1.0;
    constexpr float TWO_64          = 2.0;
    constexpr float THREE_64        = 3.0;
    constexpr float FOUR_64         = 4.0;
    constexpr float FIVE_64         = 5.0;
    constexpr float TEN_64          = 10.0;
    constexpr float HUNDRED_64      = 100.0;
    constexpr float THOUSAND_64     = 1000.0;
    constexpr float HALF_64         = 0.5;
    constexpr float QUARTER_64      = 0.25;
    constexpr float THIRD_64        = 0.3333333333333333333333333333333;

    // Integers
    constexpr int ZERO_I32          = 0;
    constexpr int ONE_I32           = 1;
    constexpr int NEG_ONE_I32       = -1;
    constexpr int TWO_I32           = 2;
    constexpr int THREE_I32         = 3;
} // namespace gkit::math