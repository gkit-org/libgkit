#include "core/input/cache.hpp"
#include <SDL3/SDL.h>


auto gkit::input::Cache::update_cache() -> void {
    this->previous_cache = this->current_cache;
    auto& frame = this->current_cache;

    SDL_Event e;
    while(SDL_PollEvent(&e)) {
        switch (e.type) {
            case SDL_EVENT_KEY_DOWN: {
                auto key = static_cast<Key>(e.key.scancode);
                frame.key_cache.pressed_keys.insert(key);
                frame.key_cache.pressed_modifiers |= e.key.mod;
                break;
            }

            case SDL_EVENT_KEY_UP: {
                auto key = static_cast<Key>(e.key.scancode);
                frame.key_cache.pressed_keys.erase(key);
                frame.key_cache.pressed_modifiers &= ~e.key.mod;
                break;
            }

            default:
                break;
        }
    }
}