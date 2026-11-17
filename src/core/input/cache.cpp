#include "core/input/cache.hpp"

#include <stdexcept>

namespace gkit::input {
    auto Cache::initialize() -> void {
        if (!SDL_IsMainThread() || !(SDL_WasInit(SDL_INIT_EVENTS) & SDL_INIT_EVENTS)) {
            throw std::logic_error("initialize input on the SDL main thread after SDL_INIT_EVENTS");
        }
        if (!this->subscriptions.empty()) return;
        auto& dispatcher = core::SDLEventDispatcher::instance();
        this->subscriptions.reserve(16);
        try {
            this->subscriptions.push_back(dispatcher.register_handler_before_frame([this] {
                this->current.previous_keys      = this->current.keys;
                this->current.previous_buttons   = this->current.buttons;
                this->current.previous_modifiers = this->current.modifiers;
                this->current.pressed_keys.clear();
                this->current.released_keys.clear();
                this->current.pressed_buttons.clear();
                this->current.released_buttons.clear();
                this->current.pressed_gamepad.clear();
                this->current.released_gamepad.clear();
                this->current.motion = {};
                this->current.wheel  = {};
            }));
            auto on = [&](SDL_EventType type, auto callback) {
                this->subscriptions.push_back(dispatcher.register_event_handler(type, callback));
            };
            on(SDL_EVENT_KEY_DOWN, [this](const SDL_Event& e) {
                auto key = static_cast<Key>(e.key.scancode);
                if (this->current.keys.insert(key).second) this->current.pressed_keys.insert(key);
                this->current.modifiers = e.key.mod;
            });
            on(SDL_EVENT_KEY_UP, [this](const SDL_Event& e) {
                auto key = static_cast<Key>(e.key.scancode);
                if (this->current.keys.erase(key)) this->current.released_keys.insert(key);
                this->current.modifiers = e.key.mod;
            });
            on(SDL_EVENT_MOUSE_BUTTON_DOWN, [this](const SDL_Event& e) {
                auto button = static_cast<MouseButton>(e.button.button);
                if (this->current.buttons.insert(button).second) this->current.pressed_buttons.insert(button);
            });
            on(SDL_EVENT_MOUSE_BUTTON_UP, [this](const SDL_Event& e) {
                auto button = static_cast<MouseButton>(e.button.button);
                if (this->current.buttons.erase(button)) this->current.released_buttons.insert(button);
            });
            on(SDL_EVENT_MOUSE_MOTION, [this](const SDL_Event& e) {
                this->current.motion.x += e.motion.xrel;
                this->current.motion.y += e.motion.yrel;
            });
            on(SDL_EVENT_MOUSE_WHEEL, [this](const SDL_Event& e) {
                this->current.wheel.x += e.wheel.x;
                this->current.wheel.y += e.wheel.y;
            });
            on(SDL_EVENT_GAMEPAD_BUTTON_DOWN, [this](const SDL_Event& e) {
                if (this->current.gamepad.insert(e.gbutton.button).second)
                    this->current.pressed_gamepad.insert(e.gbutton.button);
            });
            on(SDL_EVENT_GAMEPAD_BUTTON_UP, [this](const SDL_Event& e) {
                if (this->current.gamepad.erase(e.gbutton.button))
                    this->current.released_gamepad.insert(e.gbutton.button);
            });
            on(SDL_EVENT_WINDOW_FOCUS_LOST, [this](const SDL_Event&) {
                this->current.released_keys.insert(this->current.keys.begin(), this->current.keys.end());
                this->current.released_buttons.insert(this->current.buttons.begin(), this->current.buttons.end());
                this->current.keys.clear();
                this->current.buttons.clear();
                this->current.modifiers = 0;
            });
            this->subscriptions.push_back(dispatcher.register_handler_after_frame([this] {
                this->published.store(std::make_shared<const Snapshot>(this->current));
            }));
        } catch (...) {
            for (auto token : this->subscriptions) dispatcher.disconnect(token);
            this->subscriptions.clear();
            throw;
        }
    }
    Cache::~Cache() {
        for (auto token : this->subscriptions) core::SDLEventDispatcher::instance().disconnect(token);
    }
} // namespace gkit::input
