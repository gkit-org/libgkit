#include "core/input/cache.hpp"

namespace gkit::input {
    namespace {
        template<class Set, class Key>
        auto transition(Set& held, Set& pressed, Set& released, Key key, bool down) -> void {
            if (down) {
                if (held.insert(key).second) pressed.insert(key);
            } else if (held.erase(key))
                released.insert(key);
        }
    } // namespace

    auto Cache::initialize() -> void {
        platform::require_main_thread();
        std::unique_lock lock(mutex);
        keys.clear();
        buttons.clear();
        gamepad.clear();
        modifiers = 0;
        begin_frame();
    }


    auto Cache::begin_frame() -> void {
        previous_keys      = keys;
        previous_buttons   = buttons;
        previous_modifiers = modifiers;
        pressed_keys.clear();
        released_keys.clear();
        pressed_buttons.clear();
        released_buttons.clear();
        pressed_gamepad.clear();
        released_gamepad.clear();
        motion = {};
        wheel  = {};
    }


    auto Cache::update(std::span<const platform::Event> events) -> void {
        platform::require_main_thread();
        std::unique_lock lock(mutex);
        begin_frame();
        for (const auto& event : events) {
            std::visit(
                [&](const auto& e) -> void {
                    using T = std::decay_t<decltype(e)>;
                    if constexpr (std::is_same_v<T, platform::KeyEvent>) {
                        transition(keys, pressed_keys, released_keys, static_cast<Key>(e.scancode), e.down);
                        modifiers = e.modifiers;
                    } else if constexpr (std::is_same_v<T, platform::MouseButtonEvent>) {
                        transition(
                            buttons, pressed_buttons, released_buttons, static_cast<MouseButton>(e.button), e.down);
                    } else if constexpr (std::is_same_v<T, platform::GamepadButtonEvent>) {
                        transition(gamepad, pressed_gamepad, released_gamepad, static_cast<int>(e.button), e.down);
                    } else if constexpr (std::is_same_v<T, platform::MotionEvent>) {
                        motion.x += e.x;
                        motion.y += e.y;
                    } else if constexpr (std::is_same_v<T, platform::WheelEvent>) {
                        wheel.x += e.x;
                        wheel.y += e.y;
                    } else if constexpr (std::is_same_v<T, platform::WindowEvent>) {
                        if (e.change == platform::WindowChange::FocusLost) {
                            released_keys.insert(keys.begin(), keys.end());
                            released_buttons.insert(buttons.begin(), buttons.end());
                            keys.clear();
                            buttons.clear();
                            modifiers = 0;
                        }
                    }
                },
                event);
        }
    }
} // namespace gkit::input
