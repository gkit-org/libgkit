#include "sdl_event_dispatcher.hpp"


auto gkit::misc::SDLEventDispatcher::register_handler_before_frame(std::function<void(void)> handler) -> void {
    // Ignore empty callbacks to avoid extra checks at call sites.
    if (!handler) {
        return;
    }

    this->before_frame_callbacks.push_back(std::move(handler));
}


auto gkit::misc::SDLEventDispatcher::register_handler_after_frame(std::function<void(void)> handler) -> void {
    // Ignore empty callbacks to avoid extra checks at call sites.
    if (!handler) {
        return;
    }

    this->after_frame_callbacks.push_back(std::move(handler));
}


auto gkit::misc::SDLEventDispatcher::register_event_handler(
    SDL_EventType event_type,
    std::function<void(const SDL_Event&)> handler
) -> void {
    // Ignore empty callbacks to keep dispatch loop simple.
    if (!handler) {
        return;
    }

    this->event_handlers[event_type].push_back(std::move(handler));
}


auto gkit::misc::SDLEventDispatcher::unregister_event_handler(SDL_EventType event_type) -> void {
    // Remove the whole callback chain for this event type.
    this->event_handlers.erase(event_type);
}


auto gkit::misc::SDLEventDispatcher::dispatch_events() -> void {
    // 1) Run frame-begin callbacks before consuming SDL events.
    for (auto& callback : this->before_frame_callbacks) {
        if (callback) {
            callback();
        }
    }

    SDL_Event event = {};

    while (SDL_PollEvent(&event)) {
        auto handler_it = this->event_handlers.find(event.type);
        if (handler_it == this->event_handlers.end()) {
            continue;
        }

        auto& handlers = handler_it->second;
        // Dispatch to all handlers in registration order.
        for (auto& handler : handlers) {
            if (handler) {
                handler(event);
            }
        }
    }

    // 3) Run frame-end callbacks after all queued SDL events are processed.
    for (auto& callback : this->after_frame_callbacks) {
        if (callback) {
            callback();
        }
    }
}
