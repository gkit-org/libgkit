#pragma once

#include <cstdint>
#include <limits>

namespace gkit::math {

    /// @brief Scalar limits for numeric types
    /// @tparam T Numeric type
    template<typename T>
    struct ScalarLimits {
        /// @brief Minimum value
        static constexpr T min_v    = std::numeric_limits<T>::min();

        /// @brief Maximum value
        static constexpr T max_v    = std::numeric_limits<T>::max();

        /// @brief Lowest value (most negative)
        static constexpr T lowest_v = std::numeric_limits<T>::lowest();

        /// @brief Machine epsilon
        static constexpr T epsilon_v = std::numeric_limits<T>::epsilon();
    };

    // Floating-point special values (32-bit float)
    /// @brief Float epsilon (machine epsilon for float)
    constexpr float EPSILON32 = std::numeric_limits<float>::epsilon();

    /// @brief Float minimum positive value
    constexpr float MIN32     = std::numeric_limits<float>::min();

    /// @brief Float maximum value
    constexpr float MAX32     = std::numeric_limits<float>::max();

    // Floating-point special values (64-bit double)
    /// @brief Double epsilon (machine epsilon for double)
    constexpr float EPSILON64 = std::numeric_limits<float>::epsilon();

    /// @brief Double minimum positive value
    constexpr float MIN64     = std::numeric_limits<float>::min();

    /// @brief Double maximum value
    constexpr float MAX64     = std::numeric_limits<float>::max();

    // Integer special values (8-bit)
    /// @brief 8-bit signed minimum (-128)
    constexpr int8_t  I8_MIN  = std::numeric_limits<int8_t>::min();

    /// @brief 8-bit signed maximum (127)
    constexpr int8_t  I8_MAX  = std::numeric_limits<int8_t>::max();

    /// @brief 8-bit unsigned maximum (255)
    constexpr uint8_t U8_MAX = std::numeric_limits<uint8_t>::max();

    // Integer special values (16-bit)
    /// @brief 16-bit signed minimum
    constexpr int16_t I16_MIN = std::numeric_limits<int16_t>::min();

    /// @brief 16-bit signed maximum
    constexpr int16_t I16_MAX = std::numeric_limits<int16_t>::max();

    /// @brief 16-bit unsigned maximum
    constexpr uint16_t U16_MAX = std::numeric_limits<uint16_t>::max();

    // Integer special values (32-bit)
    /// @brief 32-bit signed minimum
    constexpr int32_t I32_MIN = std::numeric_limits<int32_t>::min();

    /// @brief 32-bit signed maximum
    constexpr int32_t I32_MAX = std::numeric_limits<int32_t>::max();

    /// @brief 32-bit unsigned maximum
    constexpr uint32_t U32_MAX = std::numeric_limits<uint32_t>::max();

    // Integer special values (64-bit)
    /// @brief 64-bit signed minimum
    constexpr int64_t I64_MIN = std::numeric_limits<int64_t>::min();

    /// @brief 64-bit signed maximum
    constexpr int64_t I64_MAX = std::numeric_limits<int64_t>::max();

    /// @brief 64-bit unsigned maximum
    constexpr uint64_t U64_MAX = std::numeric_limits<uint64_t>::max();
} // namespace gkit::math
