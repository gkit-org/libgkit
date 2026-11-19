#include "./min_window_for_input_test.hpp"
#include "gkit/platform/runtime.hpp"
#include "test_utils.hpp"

#include <cstdint>
#include <iostream>

#include <gkit/core/input/input.hpp>

auto test_key_input_loop() -> bool {
    using gkit::input::Action;
    using gkit::input::Key;
    using gkit::input::KeyChord;
    using gkit::input::Mod;

    gkit::test::logln("=== key input interactive loop (press Ctrl+S to save, Q to quit) ===");

    gkit::test::Window win;
    gkit::platform::Runtime runtime;
    auto& input = gkit::Input::instance();
    input.initialize();

    auto save_action = Action("Save");
    save_action.set_action(KeyChord{.keys = {Key::S}, .modifiers = static_cast<uint32_t>(Mod::LCtrl)});

    auto quit_action = Action("Quit");
    quit_action.set_action(KeyChord{.keys = {Key::Q}});

    input.register_action(save_action);
    input.register_action(quit_action);

    while (1) {
        input.update(runtime.poll_events());

        if (input.is_key_pressed(Key::A)) {
            std::cout << "Key A is pressed" << '\n';
        }

        if (input.is_key_just_pressed(Key::Space)) {
            std::cout << "Key Space is just pressed" << '\n';
        }

        if (input.is_action_pressed("Quit")) {
            std::cout << "Action Quit is pressed, exiting..." << '\n';
            break;
        }

        if (input.is_action_just_pressed("Save")) {
            std::cout << "Action Save is just pressed" << '\n';
        }
    }

    return true;
}


auto main() -> int {
    auto test_runner = gkit::test::TestRunner().add_test_func(test_key_input_loop);

    test_runner.run();
    return 0;
}
