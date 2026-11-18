#include "platform/sdl_event_dispatcher.hpp"

#include <stdexcept>

namespace gkit::platform::detail {
    auto SDLEventDispatcher::add(Uint32 type, std::function<void(const SDL_Event&)> handler) -> Token {
        if (!handler) return 0;
        auto entry      = std::make_shared<Entry>();
        entry->type     = type;
        entry->callback = std::move(handler);
        std::lock_guard guard(this->registry_mutex);
        auto token = this->next_token++;
        this->entries.emplace(token, std::move(entry));
        return token;
    }
    auto SDLEventDispatcher::register_handler_before_frame(std::function<void()> handler) -> Token {
        if (!handler) return 0;
        return this->add(BEFORE_FRAME, [handler = std::move(handler)](const auto&) {
            handler();
        });
    }
    auto SDLEventDispatcher::register_handler_after_frame(std::function<void()> handler) -> Token {
        if (!handler) return 0;
        return this->add(AFTER_FRAME, [handler = std::move(handler)](const auto&) {
            handler();
        });
    }
    auto SDLEventDispatcher::register_event_handler(SDL_EventType type, std::function<void(const SDL_Event&)> handler)
        -> Token {
        return this->add(type, std::move(handler));
    }

    auto SDLEventDispatcher::disconnect(Token token) -> void {
        std::shared_ptr<Entry> entry;
        {
            std::lock_guard guard(this->registry_mutex);
            auto it = this->entries.find(token);
            if (it == this->entries.end()) return;
            entry = std::move(it->second);
            this->entries.erase(it);
        }
        std::lock_guard guard(entry->execution_mutex);
        entry->enabled = false;
    }
    auto SDLEventDispatcher::unregister_event_handler(SDL_EventType type) -> void {
        std::vector<Token> tokens;
        {
            std::lock_guard guard(this->registry_mutex);
            for (const auto& [token, entry] : this->entries)
                if (entry->type == type) tokens.push_back(token);
        }
        for (auto token : tokens) this->disconnect(token);
    }
    auto SDLEventDispatcher::dispatch(Uint32 type, const SDL_Event& event) -> void {
        std::vector<std::shared_ptr<Entry>> callbacks;
        {
            std::lock_guard guard(this->registry_mutex);
            for (const auto& [token, entry] : this->entries)
                if (entry->type == type) callbacks.push_back(entry);
        }
        for (const auto& entry : callbacks) {
            std::lock_guard guard(entry->execution_mutex);
            if (entry->enabled) entry->callback(event);
        }
    }
    auto SDLEventDispatcher::dispatch_events(const std::function<void(const SDL_Event&)>& collect) -> void {
        if (!SDL_IsMainThread()) throw std::logic_error("SDL events must be dispatched on the main thread");
        SDL_Event event{};
        this->dispatch(BEFORE_FRAME, event);
        while (SDL_PollEvent(&event)) {
            collect(event);
            this->dispatch(event.type, event);
        }
        this->dispatch(AFTER_FRAME, event);
    }
} // namespace gkit::platform::detail
