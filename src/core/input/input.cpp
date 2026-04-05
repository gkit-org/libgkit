#include "gkit/core/input/input.hpp"
#include "core/input/cache.hpp"
#include <SDL3/SDL.h>


gkit::Input::Input() {
    SDL_Init(SDL_INIT_EVENTS);
}


auto gkit::Input::update() -> void {
    gkit::input::Cache::instance().update_cache();
}


auto gkit::Input::is_key_pressed(gkit::input::Key key) -> bool {
    return gkit::input::Cache::instance().current_cache.key_cache.pressed_keys.contains(key);
}


auto gkit::Input::is_key_released(gkit::input::Key key) -> bool {
    return !gkit::input::Cache::instance().current_cache.key_cache.pressed_keys.contains(key);
}


auto gkit::Input::is_key_just_pressed(gkit::input::Key key) -> bool {
    auto& cache = gkit::input::Cache::instance();
    return cache.current_cache.key_cache.pressed_keys.contains(key) &&
         !cache.previous_cache.key_cache.pressed_keys.contains(key);
}

auto gkit::Input::is_key_just_released(gkit::input::Key key) -> bool {
    auto& cache = gkit::input::Cache::instance();
    return !cache.current_cache.key_cache.pressed_keys.contains(key) &&
           cache.previous_cache.key_cache.pressed_keys.contains(key);
}