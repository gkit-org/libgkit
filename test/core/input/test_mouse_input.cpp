#include "./min_window_for_input_test.hpp"
#include "gkit/platform/runtime.hpp"
#include "test_utils.hpp"

#include <cstdint>
#include <cstdio>
#include <iostream>

#include <gkit/core/input/input.hpp>

auto test_mouse_input_loop() -> bool {
    using gkit::input::Action;
    using gkit::input::Mod;
    using gkit::input::MouseChord;

    gkit::test::logln("=== mouse input interactive loop (press Q to quit) ===");

    auto win    = gkit::test::Window{};
    auto& input = gkit::Input::instance();
    input.initialize();
    gkit::platform::Runtime runtime;

    auto shoot_action = Action("Shoot");
    shoot_action.set_action(
        MouseChord{.buttons = {gkit::input::MouseButton::Left}, .modifiers = static_cast<uint32_t>(Mod::None)});

    auto aim_action = Action("Aim");
    aim_action.set_action(
        MouseChord{.buttons = {gkit::input::MouseButton::Right}, .modifiers = static_cast<uint32_t>(Mod::None)});

    auto jump_url_action = Action("JumpUrl");
    jump_url_action.set_action(
        MouseChord{.buttons = {gkit::input::MouseButton::Left}, .modifiers = static_cast<uint32_t>(Mod::LCtrl)});

    input.register_action(shoot_action);
    input.register_action(aim_action);
    input.register_action(jump_url_action);

    while (1) {
        input.update(runtime.poll_events());

        if (input.is_action_pressed("Shoot")) {
            std::cout << "Action Shoot is pressed" << '\n';
        }

        if (input.is_mouse_button_just_pressed(gkit::input::MouseButton::Middle)) {
            std::cout << "Mouse button Middle is just pressed" << '\n';
        }

        if (input.is_action_pressed("Aim")) {
            std::cout << "Action Aim is pressed" << '\n';
        }

        if (input.is_action_pressed("JumpUrl")) {
            std::cout << "Action JumpUrl is pressed" << '\n';
        }

        if (input.is_key_just_pressed(gkit::input::Key::Q)) {
            std::cout << "Key Q is just pressed, exiting..." << '\n';
            break;
        }

        auto mouse_move  = input.get_mouse_move();
        auto mouse_wheel = input.get_mouse_wheel();

        if (mouse_move != gkit::math::Vector2::zero()) {
            auto [x, y] = mouse_move.properties();
            std::printf("Mouse move x = %f, y = %f\n", x, y);
        }

        if (mouse_wheel != gkit::math::Vector2::zero()) {
            auto [x, y] = mouse_wheel.properties();
            std::printf("Mouse wheel x = %f, y = %f\n", x, y);
        }
    }

    return true;
}

auto main() -> int {
    auto test_runner = gkit::test::TestRunner().add_test_func(test_mouse_input_loop);

    test_runner.run();
    return 0;
}
