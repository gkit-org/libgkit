#pragma once

#include "gkit/core/input/keys.hpp"
#include "gkit/core/scene/singleton.hpp"
#include <unordered_set>

namespace gkit {
    class Input;
}

namespace gkit::input {
    class Cache : public gkit::scene::Singleton<Cache> {
        friend class gkit::Input;
    
    public:
        struct KeyCache_t {
            std::unordered_set<Key> pressed_keys;
            std::uint32_t pressed_modifiers;
        };

        struct MouseCache_t {

        };

        struct GamepadCache_t {

        };

        struct CacheData {
            KeyCache_t key_cache = {{}, 0};
            MouseCache_t mouse_button_cache = {};
            GamepadCache_t gamepad_button_cache = {};
        };

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
            if (mods == 0u) return true;
            return (mods & this->current_cache.key_cache.pressed_modifiers) == mods;
        }

    private:
        CacheData current_cache;
        CacheData previous_cache;
    };
}
