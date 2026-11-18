#pragma once

#include "gkit/core/input/action.hpp"
#include "gkit/core/templates/singleton.hpp"
#include "mouse.hpp"

#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>
#include <unordered_set>

namespace gkit::input {
    /** @brief Immutable once published; retain one snapshot for a consistent frame read. */
    struct Snapshot {
        std::unordered_set<Key> keys, pressed_keys, released_keys, previous_keys;
        std::unordered_set<MouseButton> buttons, pressed_buttons, released_buttons, previous_buttons;
        std::unordered_set<int> gamepad, pressed_gamepad, released_gamepad;
        MouseMove motion;
        MouseWheel wheel;
        std::uint32_t modifiers          = 0;
        std::uint32_t previous_modifiers = 0;
    };
} // namespace gkit::input

namespace gkit {
    class Input : public gkit::core::templates::Singleton<Input> {
    public:
        Input();
        /** @brief Call on the SDL main thread after SDL event initialization. */
        auto initialize() -> void;
        auto snapshot() const -> std::shared_ptr<const input::Snapshot>;
        virtual ~Input() = default;

    public:
        // action related
        auto register_action(const input::Action& action) -> void;
        auto unregister_action(const std::string& name) -> void;

        [[nodiscard]] auto is_action_pressed(const std::string& name) const -> bool;
        [[nodiscard]] auto is_action_released(const std::string& name) const -> bool;
        [[nodiscard]] auto is_action_just_pressed(const std::string& name) const -> bool;
        [[nodiscard]] auto is_action_just_released(const std::string& name) const -> bool;

        // key related
        [[nodiscard]] auto is_key_pressed(gkit::input::Key key) const -> bool;
        [[nodiscard]] auto is_key_released(gkit::input::Key key) const -> bool;
        [[nodiscard]] auto is_key_just_pressed(gkit::input::Key key) const -> bool;
        [[nodiscard]] auto is_key_just_released(gkit::input::Key key) const -> bool;

        // mouse button related
        [[nodiscard]] auto is_mouse_button_pressed(input::MouseButton button) const -> bool;
        [[nodiscard]] auto is_mouse_button_released(input::MouseButton button) const -> bool;
        [[nodiscard]] auto is_mouse_button_just_pressed(input::MouseButton button) const -> bool;
        [[nodiscard]] auto is_mouse_button_just_released(input::MouseButton button) const -> bool;

        [[nodiscard]] auto get_mouse_move() const -> input::MouseMove;
        [[nodiscard]] auto get_mouse_wheel() const -> input::MouseWheel;

        // gamepad button related
        [[nodiscard]] auto is_gamepad_button_pressed(int button) const -> bool;
        [[nodiscard]] auto is_gamepad_button_released(int button) const -> bool;
        [[nodiscard]] auto is_gamepad_button_just_pressed(int button) const -> bool;
        [[nodiscard]] auto is_gamepad_button_just_released(int button) const -> bool;

    private:
        mutable std::mutex action_mutex;
        std::unordered_map<std::string, input::Action> action_map;
        auto action_state(const std::string& name, int edge) const -> bool;
    }; // class Input
} // namespace gkit
