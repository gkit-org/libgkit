#pragma once

#include <SDL3/SDL.h>

#include <functional>
#include <map>
#include <memory>
#include <mutex>
#include <vector>

namespace gkit::platform::detail {
    /** @brief Thread-safe subscriptions; dispatch must run on the SDL main thread. */
    class SDLEventDispatcher {
        SDLEventDispatcher() = default;

    public:
        static auto instance() -> SDLEventDispatcher& {
            static SDLEventDispatcher instance;
            return instance;
        }
        using Token = std::uint64_t;
        auto register_handler_before_frame(std::function<void()> handler) -> Token;
        auto register_handler_after_frame(std::function<void()> handler) -> Token;
        auto register_event_handler(SDL_EventType type, std::function<void(const SDL_Event&)> handler) -> Token;
        auto disconnect(Token token) -> void;
        auto unregister_event_handler(SDL_EventType type) -> void;
        auto dispatch_events(const std::function<void(const SDL_Event&)>& collect) -> void;

    private:
        struct Entry {
            // Recursive to support self-disconnect; never held with the registry mutex.
            std::recursive_mutex execution_mutex;
            bool enabled = true;
            Uint32 type;
            std::function<void(const SDL_Event&)> callback;
        };
        std::mutex registry_mutex;
        std::map<Token, std::shared_ptr<Entry>> entries;
        Token next_token                     = 1;
        static constexpr Uint32 BEFORE_FRAME = 0;
        static constexpr Uint32 AFTER_FRAME  = 1;
        auto add(Uint32 type, std::function<void(const SDL_Event&)> handler) -> Token;
        auto dispatch(Uint32 type, const SDL_Event& event) -> void;
    };
} // namespace gkit::platform::detail
