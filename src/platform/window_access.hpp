#pragma once
#include "gkit/platform/window.hpp"

#include <SDL3/SDL_video.h>

namespace gkit::platform::detail {
    struct WindowAccess {
        static auto native(Window& window) -> SDL_Window*;
    };
} // namespace gkit::platform::detail
