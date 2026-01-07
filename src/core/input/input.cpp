#include "core/include/input/input.hpp"
#include "core/include/input/keydef.hpp"
#include <SDL3/SDL.h>
#include <SDL3/SDL_events.h>
#include <string>
#include <vector>

gkit::Input::Input() noexcept : Singleton<Input>() {
    if (!SDL_WasInit(SDL_INIT_EVENTS)) {
        if (!SDL_Init(SDL_INIT_EVENTS))
            SDL_LogError(SDL_LOG_CATEGORY_ERROR, "failed to init input system, error: %s", SDL_GetError());
    }
}

void gkit::Input::bind_input_event(std::string event_name, std::vector<gkit::KeyCombination>& keys) noexcept {
    if (this->input_map.find(event_name) == this->input_map.end()) {
        this->input_map.insert(std::make_pair(event_name, keys));
    } else {
        for (auto& key : keys) {
            this->input_map[event_name].push_back(key);
        }
    }
}

void gkit::Input::bind_input_event(std::string event_name, std::vector<gkit::KeyCombination>&& keys) noexcept {
    this->bind_input_event(event_name, keys);
}

bool gkit::Input::is_event_pressed(std::string event_name) noexcept {
    if (this->input_event_cache.find(event_name) == this->input_event_cache.end()) {
        return false;
    } else {
        return true;
    }
}