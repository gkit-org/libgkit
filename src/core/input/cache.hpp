#pragma once

#include "gkit/core/input/keys.hpp"
#include "gkit/core/input/mouse.hpp"
#include "gkit/core/scene/singleton.hpp"
#include "gkit/math/vector2.hpp"
#include <SDL3/SDL.h>
#include <cstdint>
#include <unordered_set>

namespace gkit {
    class Input;
}

namespace gkit::input {
    class Cache : public gkit::scene::Singleton<Cache> {
        friend class gkit::Input;
    public:
        /**
         * @brief Update the input cache by polling SDL events and updating the current and previous cache states.
         */
        auto update_cache() -> void;

    public:
        /**
         * @brief Check if the specified modifiers are currently pressed.
         * @param mods The modifier bitmask to check
         * @return true if all specified modifiers are currently pressed, false otherwise. 
         * if mods is 0, it will return true.
         */
        inline auto modifiers_pressed(uint32_t mods) const -> bool {
            const auto& current_mods = static_cast<uint32_t>(SDL_GetModState());
            if (mods == 0u) return current_mods == 0u;
            return (mods & current_mods) == mods;
        }

    private:
        struct KeyCache_t {
            std::unordered_set<Key> pressed_keys;
            /* std::uint32_t pressed_modifiers; */
        };

        struct MouseCache_t {
            std::unordered_set<MouseButton> pressed_buttons;
            MouseWheel wheel;
            math::Vector2 offset;
        };

        struct GamepadCache_t {

        };

        struct CacheData {
            KeyCache_t key_cache = {};
            MouseCache_t mouse_button_cache = {};
            GamepadCache_t gamepad_button_cache = {};
        };

    private:
        CacheData current_cache;
        CacheData previous_cache;
    };
}
