#include "gkit/core/input/input.hpp"
#include "core/input/cache.hpp"
#include "gkit/core/input/mouse.hpp"
#include <SDL3/SDL.h>
#include <type_traits>
#include <variant>
#include <iostream>


gkit::Input::Input() {
    SDL_Init(SDL_INIT_EVENTS);
    if (!SDL_EventEnabled(SDL_EVENT_MOUSE_BUTTON_DOWN)) {
        std::cerr << "Failed to enable SDL mouse button events: " << SDL_GetError() << std::endl;
    }
}


auto gkit::Input::register_action(const input::Action& action) -> void {
    if (action.name.empty()) return;
    this->action_map[action.name] = action;
}


auto gkit::Input::unregister_action(const std::string& name) -> void {
    if (!name.empty() && this->action_map.contains(name)) {
        this->action_map.erase(name);
    }
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


auto gkit::Input::is_mouse_button_pressed(input::MouseButton button) -> bool {
    return gkit::input::Cache::instance().current_cache.mouse_cache.pressed_buttons.contains(button);
}


auto gkit::Input::is_mouse_button_released(input::MouseButton button) -> bool {
    return !gkit::input::Cache::instance().current_cache.mouse_cache.pressed_buttons.contains(button);
}


auto gkit::Input::is_mouse_button_just_pressed(input::MouseButton button) -> bool {
    auto& cache = gkit::input::Cache::instance();
    return cache.current_cache.mouse_cache.pressed_buttons.contains(button) &&
         !cache.previous_cache.mouse_cache.pressed_buttons.contains(button);
}


auto gkit::Input::is_mouse_button_just_released(input::MouseButton button) -> bool {
    auto& cache = gkit::input::Cache::instance();
    return !cache.current_cache.mouse_cache.pressed_buttons.contains(button) &&
           cache.previous_cache.mouse_cache.pressed_buttons.contains(button);
}


auto gkit::Input::get_mouse_move() -> input::MouseMove {
    auto& cache = gkit::input::Cache::instance();
    return cache.current_cache.mouse_cache.offset;
}



auto gkit::Input::get_mouse_wheel() -> input::MouseWheel {
    auto& cache = gkit::input::Cache::instance();
    return cache.current_cache.mouse_cache.wheel;
}


auto gkit::Input::is_action_pressed(std::string name) -> bool {
    auto action_it = this->action_map.find(name);
    if (action_it == this->action_map.end()) {
        return false;
    }

    auto& action = action_it->second;
    return std::visit([this](const auto& chord) -> bool {
        using ChordT = std::decay_t<decltype(chord)>;

        if constexpr (std::is_same_v<input::KeyChord, ChordT>) {
            /** Type is KeyChord */ 
            if (chord.keys.empty()) {
                return false;
            }

            for (const auto& key : chord.keys) {
                if (!this->is_key_pressed(key)) {
                    return false;
                }
            }

            return gkit::input::Cache::instance().modifiers_pressed(chord.modifiers);
        } else if constexpr (std::is_same_v<input::MouseChord, ChordT>) {
            /** Type is MouseChord */
            if (chord.buttons.empty()) {
                return false;
            }

            for (const auto& button : chord.buttons) {
                if (!this->is_mouse_button_pressed(button)) {
                    return false;
                }
            }

            return gkit::input::Cache::instance().modifiers_pressed(chord.modifiers);
        } 

        return false;
    }, action.chord);
}


auto gkit::Input::is_action_just_pressed(std::string name) -> bool {
    auto action_it = this->action_map.find(name);
    if (action_it == this->action_map.end()) {
        return false;
    }

    auto& action = action_it->second;
    return std::visit([this](const auto& chord) -> bool {
        using ChordT = std::decay_t<decltype(chord)>;

        if constexpr (std::is_same_v<input::KeyChord, ChordT>) {
            /** Type is KeyChord */ 
            if (chord.keys.empty()) {
                return false;
            }

            for (const auto& key : chord.keys) {
                if (!this->is_key_just_pressed(key)) {
                    return false;
                }
            }

            return gkit::input::Cache::instance().modifiers_pressed(chord.modifiers);
        } else if constexpr (std::is_same_v<input::MouseChord, ChordT>) {
            /** Type is MouseChord */
            if (chord.buttons.empty()) {
                return false;
            }

            for (const auto& button : chord.buttons) {
                if (!this->is_mouse_button_just_pressed(button)) {
                    return false;
                }
            }

            return gkit::input::Cache::instance().modifiers_pressed(chord.modifiers);
        }

        return false;
    }, action.chord);
}

