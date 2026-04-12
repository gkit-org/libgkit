#pragma once

#include "gkit/core/scene/singleton.hpp"
#include <SDL3/SDL.h>
#include <functional>
#include <unordered_map>
#include <vector>

namespace gkit::misc {
    class SDLEventDispatcher : public gkit::core::scene::Singleton<SDLEventDispatcher> {
    public:
        auto register_event_handler(SDL_EventType event_type, std::function<void(const SDL_Event&)> handler) -> void;
        auto unregister_event(SDL_EventType event_type) -> void;

        /**
         * @brief Dispatches SDL events to the appropriate handlers.
         * This function should be called in the main loop to ensure that all SDL events are processed.
         */
        auto dispatch_events() -> void;

    private:
        std::unordered_map<SDL_EventType, std::vector<std::function<void(const SDL_Event&)>>> event_handlers;
    };
}
