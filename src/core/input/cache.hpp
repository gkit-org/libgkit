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
        auto update_cache() -> void;

    private:
        CacheData current_cache;
        CacheData previous_cache;
    };
}
