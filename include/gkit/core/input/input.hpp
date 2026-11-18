#pragma once

#include "gkit/core/input/action.hpp"
#include "gkit/core/templates/singleton.hpp"
#include "gkit/platform/runtime.hpp"
#include "mouse.hpp"

#include <mutex>
#include <span>
#include <string>
#include <unordered_map>

namespace gkit {
    /**
     * @brief Process-wide input state updated from platform event batches.
     * @note Queries are thread-safe. Processer updates input before running the tree and waits for all
     * workers before advancing input again. Just-pressed and just-released edges last for one logic
     * frame, including every fixed physics step executed within that frame.
     */
    class Input : public gkit::core::templates::Singleton<Input> {
    public:
        Input();
        /** @brief Clear input state while keeping registered actions. Main thread only. */
        auto initialize() -> void;
        /** @brief Apply an ordered event batch and advance frame edges. Main thread only. */
        auto update(std::span<const platform::Event> events) -> void;
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
