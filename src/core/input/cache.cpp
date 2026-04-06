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
                break;
            }

            case SDL_EVENT_KEY_UP: {
                auto key = static_cast<Key>(e.key.scancode);
                frame.key_cache.pressed_keys.erase(key);
                break;
            }

            case SDL_EVENT_MOUSE_BUTTON_DOWN: {
                auto button = static_cast<MouseButton>(e.button.button);
                frame.mouse_button_cache.pressed_buttons.insert(button);
                break;
            }

            case SDL_EVENT_MOUSE_BUTTON_UP: {
                auto button = static_cast<MouseButton>(e.button.button);
                frame.mouse_button_cache.pressed_buttons.erase(button);
                break;
            }

            case SDL_EVENT_MOUSE_MOTION: {
                auto [x, y] = frame.mouse_button_cache.offset.properties();
                x = e.motion.xrel;
                y = e.motion.yrel;
                break;
            }

            case SDL_EVENT_MOUSE_WHEEL: {
                auto [x, y] = frame.mouse_button_cache.wheel.properties();
                x = e.wheel.x;
                y = e.wheel.y;
            }

            default:
                break;
        }
    }
}