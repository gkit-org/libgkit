#include "sdl_event_dispatcher.hpp"


auto gkit::misc::SDLEventDispatcher::register_event_handler(
    std::uint32_t event_type,
    std::function<void(const SDL_Event&)> handler
) -> void {
    if (!handler) {
        return;
    }

    this->event_handlers[event_type].push_back(std::move(handler));
}


auto gkit::misc::SDLEventDispatcher::unregister_event_handler(std::uint32_t event_type) -> void {
    this->event_handlers.erase(event_type);
}


auto gkit::misc::SDLEventDispatcher::dispatch_events() -> void {
    SDL_Event event = {};

    while (SDL_PollEvent(&event)) {
        auto handler_it = this->event_handlers.find(event.type);
        if (handler_it == this->event_handlers.end()) {
            continue;
        }

        auto& handlers = handler_it->second;
        for (auto& handler : handlers) {
            if (handler) {
                handler(event);
            }
        }
    }
}
