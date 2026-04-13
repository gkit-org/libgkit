#include "core/input/cache.hpp"
#include "misc/sdl_event_dispatcher.hpp"
#include <SDL3/SDL.h>


gkit::input::Cache::Cache() {
    auto& event_dispatcher = gkit::misc::SDLEventDispatcher::instance();

    // swap current and previous cache at the beginning of each frame
    // so that we can compare the two states to determine just pressed/released events.
    event_dispatcher.register_handler_before_frame([this]() {
        this->previous_cache = this->current_cache;
    });

    /************************************
     * Keyboard Up and Down Events
     ************************************/
    event_dispatcher.register_event_handler(SDL_EVENT_KEY_DOWN, [this](const SDL_Event& e) {
        auto& frame = this->current_cache;
        auto key = static_cast<Key>(e.key.scancode);
        frame.key_cache.pressed_keys.insert(key);
    });

    event_dispatcher.register_event_handler(SDL_EVENT_KEY_UP, [this](const SDL_Event& e) {
        auto& frame = this->current_cache;
        auto key = static_cast<Key>(e.key.scancode);
        frame.key_cache.pressed_keys.erase(key);
    });

    /************************************
     * Mouse Button Up and Down Events
     ************************************/
    event_dispatcher.register_event_handler(SDL_EVENT_MOUSE_BUTTON_DOWN, [this](const SDL_Event& e) {
        auto& frame = this->current_cache;
        auto button = static_cast<MouseButton>(e.button.button);
        frame.mouse_button_cache.pressed_buttons.insert(button);
    });

    event_dispatcher.register_event_handler(SDL_EVENT_MOUSE_BUTTON_UP, [this](const SDL_Event& e) {
        auto& frame = this->current_cache;
        auto button = static_cast<MouseButton>(e.button.button);
        frame.mouse_button_cache.pressed_buttons.erase(button);
    });


    /************************************
     * Mouse Motion and Wheel Events
     ************************************/
    event_dispatcher.register_event_handler(SDL_EVENT_MOUSE_MOTION, [this](const SDL_Event& e) {
        auto& frame = this->current_cache;
        auto [x, y] = frame.mouse_button_cache.offset.properties();
        x = e.motion.xrel;
        y = e.motion.yrel;
    });


    event_dispatcher.register_event_handler(SDL_EVENT_MOUSE_WHEEL, [this](const SDL_Event& e) {
        auto& frame = this->current_cache;
        auto [x, y] = frame.mouse_button_cache.wheel.properties();
        x = e.wheel.x;
        y = e.wheel.y;
    });
}



gkit::input::Cache::~Cache() {
    auto& event_dispatcher = gkit::misc::SDLEventDispatcher::instance();

    event_dispatcher.unregister_event_handler(SDL_EVENT_KEY_DOWN);
    event_dispatcher.unregister_event_handler(SDL_EVENT_KEY_UP);
    event_dispatcher.unregister_event_handler(SDL_EVENT_MOUSE_BUTTON_DOWN);
    event_dispatcher.unregister_event_handler(SDL_EVENT_MOUSE_BUTTON_UP);
    event_dispatcher.unregister_event_handler(SDL_EVENT_MOUSE_MOTION);
    event_dispatcher.unregister_event_handler(SDL_EVENT_MOUSE_WHEEL);
}