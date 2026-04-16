#pragma once

#include <cstdint>
#include <limits>

namespace gkit::math {
    // Common limits for scalar types
    template<typename T>
    struct ScalarLimits {
        static constexpr T min_v    = std::numeric_limits<T>::min();
        static constexpr T max_v    = std::numeric_limits<T>::max();
        static constexpr T lowest_v = std::numeric_limits<T>::lowest();
        static constexpr T epsilon_v = std::numeric_limits<T>::epsilon();
    };

    // Floating-point special values using numeric_limits
    constexpr float EPSILON32 = std::numeric_limits<float>::epsilon();
    constexpr float MIN32     = std::numeric_limits<float>::min();
    constexpr float MAX32     = std::numeric_limits<float>::max();

    constexpr float EPSILON64 = std::numeric_limits<float>::epsilon();
    constexpr float MIN64     = std::numeric_limits<float>::min();
    constexpr float MAX64     = std::numeric_limits<float>::max();

    // Integer special values using numeric_limits
    constexpr int8_t  I8_MIN  = std::numeric_limits<int8_t>::min();
    constexpr int8_t  I8_MAX  = std::numeric_limits<int8_t>::max();
    constexpr uint8_t U8_MAX = std::numeric_limits<uint8_t>::max();

    constexpr int16_t I16_MIN = std::numeric_limits<int16_t>::min();
    constexpr int16_t I16_MAX = std::numeric_limits<int16_t>::max();
    constexpr uint16_t U16_MAX = std::numeric_limits<uint16_t>::max();

    constexpr int32_t I32_MIN = std::numeric_limits<int32_t>::min();
    constexpr int32_t I32_MAX = std::numeric_limits<int32_t>::max();
    constexpr uint32_t U32_MAX = std::numeric_limits<uint32_t>::max();

    constexpr int64_t I64_MIN = std::numeric_limits<int64_t>::min();
    constexpr int64_t I64_MAX = std::numeric_limits<int64_t>::max();
    constexpr uint64_t U64_MAX = std::numeric_limits<uint64_t>::max();
} // namespace gkit::math