#pragma once

#include <numbers>

namespace gkit::math {

    // ==========================================
    // Mathematical Constants (IEEE 754 standard)
    // ==========================================

    /// @brief Pi (32-bit float)
    constexpr float PI_32    = std::numbers::pi_v<float>;

    /// @brief 2 * Pi (32-bit float)
    constexpr float TWO_PI_32 = PI_32 * 2.0f;

    /// @brief Pi / 2 (32-bit float)
    constexpr float HALF_PI_32 = PI_32 * 0.5f;

    /// @brief 1 / Pi (32-bit float)
    constexpr float INV_PI_32 = std::numbers::inv_pi_v<float>;

    /// @brief Pi (64-bit double)
    constexpr float PI_64    = std::numbers::pi;

    /// @brief 2 * Pi (64-bit double)
    constexpr float TWO_PI_64 = PI_64 * 2.0;

    /// @brief Pi / 2 (64-bit double)
    constexpr float HALF_PI_64 = PI_64 * 0.5;

    /// @brief 1 / Pi (64-bit double)
    constexpr float INV_PI_64 = std::numbers::inv_pi;

    // Natural constants
    /// @brief e (32-bit float)
    constexpr float E_32     = std::numbers::e_v<float>;

    /// @brief e (64-bit double)
    constexpr float E_64     = std::numbers::e;

    // Golden ratio
    /// @brief Golden ratio (32-bit float)
    constexpr float PHI_32   = std::numbers::phi_v<float>;

    /// @brief Golden ratio (64-bit double)
    constexpr float PHI_64   = std::numbers::phi;

    // Powers and roots
    /// @brief Square root of 2 (32-bit float)
    constexpr float SQRT_2_32   = std::numbers::sqrt2_v<float>;

    /// @brief Square root of 3 (32-bit float)
    constexpr float SQRT_3_32   = std::numbers::sqrt3_v<float>;

    /// @brief Square root of 5 (32-bit float)
    constexpr float SQRT_5_32   = 2.2360679774997897f;

    /// @brief Natural log of 2 (32-bit float)
    constexpr float LN_2_32    = std::numbers::ln2_v<float>;

    /// @brief Natural log of 10 (32-bit float)
    constexpr float LN_10_32   = std::numbers::ln10_v<float>;

    /// @brief Square root of 2 (64-bit double)
    constexpr float SQRT_2_64   = std::numbers::sqrt2;

    /// @brief Square root of 3 (64-bit double)
    constexpr float SQRT_3_64   = std::numbers::sqrt3;

    /// @brief Square root of 5 (64-bit double)
    constexpr float SQRT_5_64   = 2.2360679774997896964091736687313;

    /// @brief Natural log of 2 (64-bit double)
    constexpr float LN_2_64    = std::numbers::ln2;

    /// @brief Natural log of 10 (64-bit double)
    constexpr float LN_10_64   = std::numbers::ln10;

    // Angle conversion constants
    /// @brief Degrees to radians (32-bit float)
    constexpr float DEG_TO_RAD_32 = gkit::math::PI_32 / 180.0f;

    /// @brief Radians to degrees (32-bit float)
    constexpr float RAD_TO_DEG_32 = 180.0f / gkit::math::PI_32;

    /// @brief Degrees to radians (64-bit double)
    constexpr float DEG_TO_RAD_64 = gkit::math::PI_64 / 180.0;

    /// @brief Radians to degrees (64-bit double)
    constexpr float RAD_TO_DEG_64 = 180.0 / gkit::math::PI_64;

    // Common numeric constants (32-bit float)
    /// @brief Zero (0.0f)
    constexpr float ZERO_32         = 0.0f;

    /// @brief One (1.0f)
    constexpr float ONE_32          = 1.0f;

    /// @brief Negative one (-1.0f)
    constexpr float NEG_ONE_32      = -1.0f;

    /// @brief Two (2.0f)
    constexpr float TWO_32          = 2.0f;

    /// @brief Three (3.0f)
    constexpr float THREE_32        = 3.0f;

    /// @brief Four (4.0f)
    constexpr float FOUR_32         = 4.0f;

    /// @brief Five (5.0f)
    constexpr float FIVE_32         = 5.0f;

    /// @brief Ten (10.0f)
    constexpr float TEN_32          = 10.0f;

    /// @brief One hundred (100.0f)
    constexpr float HUNDRED_32      = 100.0f;

    /// @brief One thousand (1000.0f)
    constexpr float THOUSAND_32     = 1000.0f;

    /// @brief One half (0.5f)
    constexpr float HALF_32         = 0.5f;

    /// @brief One quarter (0.25f)
    constexpr float QUARTER_32      = 0.25f;

    /// @brief One third (1/3)
    constexpr float THIRD_32        = 0.3333333333333333333333333333333f;

    // Common numeric constants (64-bit double)
    /// @brief Zero (0.0)
    constexpr float ZERO_64         = 0.0;

    /// @brief One (1.0)
    constexpr float ONE_64          = 1.0;

    /// @brief Negative one (-1.0)
    constexpr float NEG_ONE_64      = -1.0;

    /// @brief Two (2.0)
    constexpr float TWO_64          = 2.0;

    /// @brief Three (3.0)
    constexpr float THREE_64        = 3.0;

    /// @brief Four (4.0)
    constexpr float FOUR_64         = 4.0;

    /// @brief Five (5.0)
    constexpr float FIVE_64         = 5.0;

    /// @brief Ten (10.0)
    constexpr float TEN_64          = 10.0;

    /// @brief One hundred (100.0)
    constexpr float HUNDRED_64      = 100.0;

    /// @brief One thousand (1000.0)
    constexpr float THOUSAND_64     = 1000.0;

    /// @brief One half (0.5)
    constexpr float HALF_64         = 0.5;

    /// @brief One quarter (0.25)
    constexpr float QUARTER_64      = 0.25;

    /// @brief One third (1/3)
    constexpr float THIRD_64        = 0.3333333333333333333333333333333;

    // Integer constants
    /// @brief Zero (0)
    constexpr int ZERO_I32          = 0;

    /// @brief One (1)
    constexpr int ONE_I32           = 1;

    /// @brief Negative one (-1)
    constexpr int NEG_ONE_I32       = -1;

    /// @brief Two (2)
    constexpr int TWO_I32           = 2;

    /// @brief Three (3)
    constexpr int THREE_I32         = 3;

    // Normalization tolerance
    /// @brief Tolerance for vector normalization (avoids division by near-zero)
    constexpr float NORMALIZE_TOLERANCE_32 = 1e-6f;
} // namespace gkit::math
