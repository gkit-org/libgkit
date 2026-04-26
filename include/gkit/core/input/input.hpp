#pragma once

#include "gkit/core/input/action.hpp"
#include "gkit/core/scene/singleton.hpp"
#include "mouse.hpp"
#include <string>
#include <unordered_map>

namespace gkit {
    class Input : public gkit::core::scene::Singleton<Input> {
    public:
        Input();
        virtual ~Input() = default;

    public:
        /********************************
         * action related
         *******************************/
        auto register_action(const input::Action& action) -> void;
        auto unregister_action(const std::string& name) -> void; 

        auto is_action_pressed(std::string name)  -> bool;
        auto is_action_released(std::string name) -> bool;
        auto is_action_just_pressed(std::string name)  -> bool;
        auto is_action_just_released(std::string name) -> bool;

        /********************************
         * key related
         *******************************/
        auto is_key_pressed(gkit::input::Key key)  -> bool;
        auto is_key_released(gkit::input::Key key) -> bool;
        auto is_key_just_pressed(gkit::input::Key key)  -> bool;
        auto is_key_just_released(gkit::input::Key key) -> bool;

        /********************************
         * mouse button related
         *******************************/
        auto is_mouse_button_pressed(input::MouseButton button)  -> bool;
        auto is_mouse_button_released(input::MouseButton button) -> bool;
        auto is_mouse_button_just_pressed(input::MouseButton button)  -> bool;
        auto is_mouse_button_just_released(input::MouseButton button) -> bool;

        /********************************
         * gamepad button related
         *******************************/
        auto is_gamepad_button_pressed(int button)  -> bool;
        auto is_gamepad_button_released(int button) -> bool;
        auto is_gamepad_button_just_pressed(int button)  -> bool;
        auto is_gamepad_button_just_released(int button) -> bool;

    private:
        std::unordered_map<std::string, input::Action> action_map;
    }; // class Input
} // namespace gkit