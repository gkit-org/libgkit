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

    // Floating-point special values using numeric_limits
    namespace fp {
        constexpr float32 EPSILON32 = std::numeric_limits<float32>::epsilon();
        constexpr float32 MIN32     = std::numeric_limits<float32>::min();
        constexpr float32 MAX32     = std::numeric_limits<float32>::max();

        constexpr float64 EPSILON64 = std::numeric_limits<float64>::epsilon();
        constexpr float64 MIN64     = std::numeric_limits<float64>::min();
        constexpr float64 MAX64     = std::numeric_limits<float64>::max();
    } // namespace fp

    // Integer special values using numeric_limits
    namespace int_limits {
        constexpr int8  I8_MIN  = std::numeric_limits<int8>::min();
        constexpr int8  I8_MAX  = std::numeric_limits<int8>::max();
        constexpr uint8 U8_MAX = std::numeric_limits<uint8>::max();

        constexpr int16 I16_MIN = std::numeric_limits<int16>::min();
        constexpr int16 I16_MAX = std::numeric_limits<int16>::max();
        constexpr uint16 U16_MAX = std::numeric_limits<uint16>::max();

        constexpr int32 I32_MIN = std::numeric_limits<int32>::min();
        constexpr int32 I32_MAX = std::numeric_limits<int32>::max();
        constexpr uint32 U32_MAX = std::numeric_limits<uint32>::max();

        constexpr int64 I64_MIN = std::numeric_limits<int64>::min();
        constexpr int64 I64_MAX = std::numeric_limits<int64>::max();
        constexpr uint64 U64_MAX = std::numeric_limits<uint64>::max();
    } // namespace int_limits
} // namespace gkit::math