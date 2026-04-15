#pragma once

#include <numbers>

#include "gkit/math/scalar.hpp"

namespace gkit::math {
    // Mathematical constants (IEEE 754 standard)
    namespace math_const {
        // Pi multiples
        constexpr float32 PI_32    = std::numbers::pi_v<float>;
        constexpr float32 TWO_PI_32 = PI_32 * 2.0f;
        constexpr float32 HALF_PI_32 = PI_32 * 0.5f;
        constexpr float32 INV_PI_32 = std::numbers::inv_pi_v<float>;

        constexpr float64 PI_64    = std::numbers::pi;
        constexpr float64 TWO_PI_64 = PI_64 * 2.0;
        constexpr float64 HALF_PI_64 = PI_64 * 0.5;
        constexpr float64 INV_PI_64 = std::numbers::inv_pi;

        // Natural constants
        constexpr float32 E_32     = std::numbers::e_v<float>;
        constexpr float64 E_64     = std::numbers::e;

        // Golden ratio
        constexpr float32 PHI_32   = std::numbers::phi_v<float>;
        constexpr float64 PHI_64   = std::numbers::phi;

        // Powers and roots
        constexpr float32 SQRT_2_32   = std::numbers::sqrt2_v<float>;
        constexpr float32 SQRT_3_32   = std::numbers::sqrt3_v<float>;
        constexpr float32 SQRT_5_32   = 2.2360679774997897f;  // sqrt(5)
        constexpr float32 LN_2_32    = std::numbers::ln2_v<float>;
        constexpr float32 LN_10_32   = std::numbers::ln10_v<float>;

        constexpr float64 SQRT_2_64   = std::numbers::sqrt2;
        constexpr float64 SQRT_3_64   = std::numbers::sqrt3;
        constexpr float64 SQRT_5_64   = 2.2360679774997896964091736687313;  // sqrt(5)
        constexpr float64 LN_2_64    = std::numbers::ln2;
        constexpr float64 LN_10_64   = std::numbers::ln10;
    } // namespace math_const

    // Angle conversion constants
    namespace angle_const {
        constexpr float32 DEG_TO_RAD_32 = gkit::math::math_const::PI_32 / 180.0f;
        constexpr float32 RAD_TO_DEG_32 = 180.0f / gkit::math::math_const::PI_32;

        constexpr float64 DEG_TO_RAD_64 = gkit::math::math_const::PI_64 / 180.0;
        constexpr float64 RAD_TO_DEG_64 = 180.0 / gkit::math::math_const::PI_64;
    } // namespace angle_const

    // Common numeric constants
    namespace numeric_const {
        constexpr float32 ZERO_32          = 0.0f;
        constexpr float32 ONE_32             = 1.0f;
        constexpr float32 NEG_ONE_32          = -1.0f;
        constexpr float32 TWO_32            = 2.0f;
        constexpr float32 THREE_32          = 3.0f;
        constexpr float32 FOUR_32          = 4.0f;
        constexpr float32 FIVE_32          = 5.0f;
        constexpr float32 TEN_32           = 10.0f;
        constexpr float32 HUNDRED_32        = 100.0f;
        constexpr float32 THOUSAND_32      = 1000.0f;
        constexpr float32 HALF_32           = 0.5f;
        constexpr float32 QUARTER_32         = 0.25f;
        constexpr float32 THIRD_32          = 0.3333333333333333333333333333333f;

        constexpr float64 ZERO_64          = 0.0;
        constexpr float64 ONE_64             = 1.0;
        constexpr float64 NEG_ONE_64          = -1.0;
        constexpr float64 TWO_64            = 2.0;
        constexpr float64 THREE_64          = 3.0;
        constexpr float64 FOUR_64           = 4.0;
        constexpr float64 FIVE_64          = 5.0;
        constexpr float64 TEN_64           = 10.0;
        constexpr float64 HUNDRED_64        = 100.0;
        constexpr float64 THOUSAND_64      = 1000.0;
        constexpr float64 HALF_64           = 0.5;
        constexpr float64 QUARTER_64         = 0.25;
        constexpr float64 THIRD_64         = 0.3333333333333333333333333333333;

        // Integers
        constexpr int32 ZERO_I32          = 0;
        constexpr int32 ONE_I32           = 1;
        constexpr int32 NEG_ONE_I32       = -1;
        constexpr int32 TWO_I32           = 2;
        constexpr int32 THREE_I32         = 3;
    } // namespace numeric_const
} // namespace gkit::math