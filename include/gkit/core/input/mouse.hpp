#pragma once

#include "gkit/core/utils/vector2.hpp"
#include <cstdint>
#include <vector>

namespace gkit::input {
    enum class MouseButton : std::uint32_t {
        Unknown = 0,
        Left    = 1,
        Middle  = 2,
        Right   = 3,
        X1      = 4,
        X2      = 5,

        ButtonCount = 6,
    }; // enum class MouseButton

    struct MouseChord {
        std::vector<MouseButton> buttons;
        uint32_t modifiers;
    }; // struct MouseChord

    using MouseWheel = gkit::utils::Vector2;
} // namespace gkit::input
