#pragma once

#include "gkit/core/scene/singleton.hpp"
#include <SDL3/SDL.h>
#include <functional>
#include <unordered_map>
#include <vector>

namespace gkit::misc {
    /**
     * @brief SDL event dispatcher based on event type to handler list mapping.
     */
    class SDLEventDispatcher : public gkit::core::scene::Singleton<SDLEventDispatcher> {
        friend class gkit::core::scene::Singleton<SDLEventDispatcher>;
    public:
        /**
         * @brief Registers a callback executed once before polling SDL events in each dispatch cycle.
         * Callbacks are appended and executed in registration order.
         */
        auto register_handler_before_frame(std::function<void(void)> handler) -> void;

        /**
         * @brief Registers a callback executed once after polling/dispatching SDL events in each dispatch cycle.
         * Callbacks are appended and executed in registration order.
         */
        auto register_handler_after_frame(std::function<void(void)> handler) -> void;

        /**
         * @brief Registers a handler for an SDL event type.
         * Handlers are appended, so one event type can have multiple handlers.
         */
        auto register_event_handler(SDL_EventType event_type, std::function<void(const SDL_Event&)> handler) -> void;

        /**
         * @brief Unregisters all handlers bound to an SDL event type.
         */
        auto unregister_event_handler(SDL_EventType event_type) -> void;

        /**
         * @brief Dispatches SDL events to the appropriate handlers.
         * This function should be called in the main loop to ensure that all SDL events are processed.
         */
        auto dispatch_events() -> void;

    private:
        // Callbacks executed before entering the SDL event polling loop.
        std::vector<std::function<void(void)>> before_frame_callbacks;

        // Callbacks executed after the SDL event polling loop finishes.
        std::vector<std::function<void(void)>> after_frame_callbacks;

        // Key: SDL event type, Value: callback chain for that event.
        std::unordered_map<Uint32, std::vector<std::function<void(const SDL_Event&)>>> event_handlers;
    }; // class SDLEventDispatcher
} // namespace gkit::misc
