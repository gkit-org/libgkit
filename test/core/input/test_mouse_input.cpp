#include "min_window_for_input_test.hpp"
#include <gkit/core/input/input.hpp>
#include <cstdint>

auto main() -> int {
    using gkit::input::Action;
    using gkit::input::Mod;
    using gkit::input::MouseChord;

    auto win = gkit::test::Window{};
    auto& input = gkit::Input::instance();

    auto shoot_action = Action("Shoot");
    shoot_action.set_action(MouseChord{
        .buttons = {gkit::input::MouseButton::Left},
        .modifiers = static_cast<uint32_t>(Mod::None)
    });

    auto aim_action = Action("Aim");
    aim_action.set_action(MouseChord{
        .buttons = {gkit::input::MouseButton::Right},
        .modifiers = static_cast<uint32_t>(Mod::None)
    });

    auto jump_url_action = Action("JumpUrl");
    jump_url_action.set_action(MouseChord{
        .buttons = {gkit::input::MouseButton::Left},
        .modifiers = static_cast<uint32_t>(Mod::LCtrl)
    });

    input.register_action(shoot_action);
    input.register_action(aim_action);
    input.register_action(jump_url_action);

    while(1) {
        input.update();

        if (input.is_action_pressed("Shoot")) {
            std::cout << "Action Shoot is pressed" << std::endl;
        }

        if (input.is_mouse_button_just_pressed(gkit::input::MouseButton::Middle)) {
            std::cout << "Mouse button Middle is just pressed" << std::endl;
        }

        if (input.is_action_pressed("Aim")) {
            std::cout << "Action Aim is pressed" << std::endl;
        }

        if (input.is_action_pressed("JumpUrl")) {
            std::cout << "Action JumpUrl is pressed" << std::endl;
        }

        if (input.is_key_just_pressed(gkit::input::Key::Q)) {
            std::cout << "Key Q is just pressed, exiting..." << std::endl;
            break;
        }
    }
}