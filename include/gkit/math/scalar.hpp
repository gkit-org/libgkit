#pragma once

#include <cstdint>
#include <limits>

namespace gkit::math {
    // Basic scalar type aliases
    using int8    = int8_t;
    using int16   = int16_t;
    using int32   = int32_t;
    using int64   = int64_t;
    using uint8   = uint8_t;
    using uint16  = uint16_t;
    using uint32  = uint32_t;
    using uint64  = uint64_t;
    using float32 = float;
    using float64 = double;

    // Common limits for scalar types
    template<typename T>
    struct ScalarLimits {
        static constexpr T min_v    = std::numeric_limits<T>::min();
        static constexpr T max_v    = std::numeric_limits<T>::max();
        static constexpr T lowest_v = std::numeric_limits<T>::lowest();
        static constexpr T epsilon_v = std::numeric_limits<T>::epsilon();
    };

    // Floating-point special values
    namespace fp {
        constexpr float32 EPSILON32    = 1.1920928955078125e-07f;      // FLT_EPSILON
        constexpr float32 MIN32     = 1.1754943508222875e-38f;   // FLT_MIN
        constexpr float32 MAX32    = 3.4028234663852886e+38f;   // FLT_MAX

        constexpr float64 EPSILON64 = 2.2204460492503131e-16;     // DBL_EPSILON
        constexpr float64 MIN64    = 2.2250738585072014e-308;    // DBL_MIN
        constexpr float64 MAX64    = 1.7976931348623157e+308;    // DBL_MAX
    } // namespace fp

    // Integer special values
    namespace int_limits {
        constexpr int8  I8_MIN  = -128;
        constexpr int8  I8_MAX  = 127;
        constexpr uint8 U8_MAX = 255;

        constexpr int16 I16_MIN = -32768;
        constexpr int16 I16_MAX = 32767;
        constexpr uint16 U16_MAX = 65535;

        constexpr int32 I32_MIN = -2147483647 - 1;
        constexpr int32 I32_MAX = 2147483647;
        constexpr uint32 U32_MAX = 4294967295u;

        constexpr int64 I64_MIN = -9223372036854775807ll - 1;
        constexpr int64 I64_MAX = 9223372036854775807ll;
        constexpr uint64 U64_MAX = 18446744073709551615ull;
    } // namespace int_limits
} // namespace gkit::math