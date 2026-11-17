#include "gkit/core/input/input.hpp"

#include "core/input/cache.hpp"

#include <algorithm>
#include <type_traits>
#include <variant>

namespace gkit {
    Input::Input() = default;
    auto Input::initialize() -> void {
        input::Cache::instance().initialize();
    }
    auto Input::snapshot() const -> std::shared_ptr<const input::Snapshot> {
        return input::Cache::instance().snapshot();
    }
    auto Input::register_action(const input::Action& action) -> void {
        if (action.name.empty()) return;
        std::lock_guard guard(this->action_mutex);
        this->action_map[action.name] = action;
    }
    auto Input::unregister_action(const std::string& name) -> void {
        std::lock_guard guard(this->action_mutex);
        this->action_map.erase(name);
    }
    auto Input::is_key_pressed(input::Key value) const -> bool {
        auto frame = this->snapshot();
        return frame->keys.contains(value);
    }
    auto Input::is_key_released(input::Key value) const -> bool {
        auto frame = this->snapshot();
        return !frame->keys.contains(value);
    }
    auto Input::is_key_just_pressed(input::Key value) const -> bool {
        auto frame = this->snapshot();
        return frame->pressed_keys.contains(value);
    }
    auto Input::is_key_just_released(input::Key value) const -> bool {
        auto frame = this->snapshot();
        return frame->released_keys.contains(value);
    }
    auto Input::is_mouse_button_pressed(input::MouseButton value) const -> bool {
        auto frame = this->snapshot();
        return frame->buttons.contains(value);
    }
    auto Input::is_mouse_button_released(input::MouseButton value) const -> bool {
        auto frame = this->snapshot();
        return !frame->buttons.contains(value);
    }
    auto Input::is_mouse_button_just_pressed(input::MouseButton value) const -> bool {
        auto frame = this->snapshot();
        return frame->pressed_buttons.contains(value);
    }
    auto Input::is_mouse_button_just_released(input::MouseButton value) const -> bool {
        auto frame = this->snapshot();
        return frame->released_buttons.contains(value);
    }
    auto Input::is_gamepad_button_pressed(int value) const -> bool {
        auto frame = this->snapshot();
        return frame->gamepad.contains(value);
    }
    auto Input::is_gamepad_button_released(int value) const -> bool {
        auto frame = this->snapshot();
        return !frame->gamepad.contains(value);
    }
    auto Input::is_gamepad_button_just_pressed(int value) const -> bool {
        auto frame = this->snapshot();
        return frame->pressed_gamepad.contains(value);
    }
    auto Input::is_gamepad_button_just_released(int value) const -> bool {
        auto frame = this->snapshot();
        return frame->released_gamepad.contains(value);
    }
    auto Input::get_mouse_move() const -> input::MouseMove {
        return this->snapshot()->motion;
    }
    auto Input::get_mouse_wheel() const -> input::MouseWheel {
        return this->snapshot()->wheel;
    }

    auto Input::action_state(const std::string& name, int edge) const -> bool {
        input::Action action;
        {
            std::lock_guard guard(this->action_mutex);
            auto it = this->action_map.find(name);
            if (it == this->action_map.end()) return false;
            action = it->second;
        }
        auto frame = this->snapshot();
        return std::visit(
            [&](const auto& chord) {
                auto evaluate = [&](const auto& members, const auto& held, const auto& previous) {
                    if (members.empty()) return false;
                    constexpr auto all_mods =
                        static_cast<std::uint32_t>(input::Mod::Alt) | static_cast<std::uint32_t>(input::Mod::Ctrl) |
                        static_cast<std::uint32_t>(input::Mod::Shift) | static_cast<std::uint32_t>(input::Mod::Gui);
                    auto matches_mods = [&](auto mods) {
                        return chord.modifiers == 0 ? (mods & all_mods) == 0
                                                    : (mods & chord.modifiers) == chord.modifiers;
                    };
                    bool down = matches_mods(frame->modifiers) && std::ranges::all_of(members, [&](auto key) {
                                    return held.contains(key);
                                });
                    bool was_down =
                        matches_mods(frame->previous_modifiers) && std::ranges::all_of(members, [&](auto key) {
                            return previous.contains(key);
                        });
                    if (edge == 2) return !down;
                    if (edge < 0) return was_down && !down;
                    return down && (edge == 0 || !was_down);
                };
                if constexpr (std::is_same_v<std::decay_t<decltype(chord)>, input::KeyChord>) {
                    return evaluate(chord.keys, frame->keys, frame->previous_keys);
                } else {
                    return evaluate(chord.buttons, frame->buttons, frame->previous_buttons);
                }
            },
            action.chord);
    }
    auto Input::is_action_pressed(const std::string& name) const -> bool {
        return this->action_state(name, 0);
    }
    auto Input::is_action_released(const std::string& name) const -> bool {
        return this->action_state(name, 2);
    }
    auto Input::is_action_just_pressed(const std::string& name) const -> bool {
        return this->action_state(name, 1);
    }
    auto Input::is_action_just_released(const std::string& name) const -> bool {
        return this->action_state(name, -1);
    }
} // namespace gkit
