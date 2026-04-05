#include "gkit/core/input/input.hpp"
#include "core/input/cache.hpp"
#include <SDL3/SDL.h>
#include <type_traits>
#include <variant>


gkit::Input::Input() {
    SDL_Init(SDL_INIT_EVENTS);
}


auto gkit::Input::update() -> void {
    gkit::input::Cache::instance().update_cache();
}


auto gkit::Input::register_action(const input::Action& action) -> void {
    this->action_map[action.name] = action;
}


auto gkit::Input::unregister_action(const std::string& name) -> void {
    if (this->action_map.contains(name)) {
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


auto gkit::Input::is_action_pressed(std::string name) -> bool {
    auto action_it = this->action_map.find(name);
    if (action_it == this->action_map.end()) {
        return false;
    }

    auto& action = action_it->second;
    return std::visit([this](const auto& chord) -> bool {
        using ChordT = std::decay_t<decltype(chord)>;

        if constexpr (std::is_same_v<input::KeyChord, ChordT>) {
            if (chord.keys.empty()) {
                return false;
            }

            for (const auto& key : chord.keys) {
                if (!this->is_key_pressed(key)) {
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
            if (chord.keys.empty()) {
                return false;
            }

            for (const auto& key : chord.keys) {
                if (!this->is_key_just_pressed(key)) {
                    return false;
                }
            }

            return gkit::input::Cache::instance().modifiers_pressed(chord.modifiers);
        }

        return false;
    }, action.chord);
}

