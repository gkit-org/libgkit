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
    class Cache : public gkit::core::scene::Singleton<Cache> {
        friend class gkit::Input;
        friend class gkit::core::scene::Singleton<Cache>;

        Cache();
        virtual ~Cache();

    public:
        /**
         * @brief Check if the specified modifiers are currently pressed.
         * @param mods The modifier bitmask to check
         * @return true if all specified modifiers are currently pressed, false otherwise. 
         * if mods is 0, it will return true when Alt, Ctrl, Shift, and Gui modifiers are not pressed.
         */
        inline auto modifiers_pressed(uint32_t mods) const -> bool {
            const auto& current_mods = static_cast<uint32_t>(SDL_GetModState());
            static constexpr const uint32_t all_mods = static_cast<uint32_t>(Mod::Alt)
                | static_cast<uint32_t>(Mod::Ctrl)
                | static_cast<uint32_t>(Mod::Shift)
                | static_cast<uint32_t>(Mod::Gui);
            
            if (mods == static_cast<uint32_t>(Mod::None)) {
                return (current_mods & all_mods) == 0u;
            }

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
