#include <cstdint>
#include <gkit/core/input/input.hpp>
#include <SDL3/SDL.h>
#include <iostream>

// A window for event system to work 
struct Window {
    Window() {
        win_ptr = SDL_CreateWindow("Test key input", 640, 480, SDL_WINDOW_RESIZABLE);
        if (!win_ptr) {
            std::cerr << "Failed to create SDL window: " << SDL_GetError() << std::endl;
            std::exit(1);
        }

        renderer = SDL_CreateRenderer(win_ptr, nullptr);
        if (!renderer) {
            std::cerr << "Failed to create SDL renderer: " << SDL_GetError() << std::endl;
            std::exit(1);
        }

        // Provide first frame for window to be ready and input system to initialize properly
        // This is necessary for the test to work correctly in Linux wayland environment
        SDL_RenderClear(renderer);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderPresent(renderer);

        SDL_ShowWindow(win_ptr);
    }

    ~Window() {
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(win_ptr);
    }

    SDL_Window* win_ptr = nullptr;
    SDL_Renderer* renderer = nullptr;
};

auto main() -> int {
    using gkit::input::Action;
    using gkit::input::Key;
    using gkit::input::Mod;
    using gkit::input::KeyChord;

    Window win;
    auto& input = gkit::Input::instance();

    auto save_action = Action("Save");
    save_action.set_action(KeyChord{
        .keys = {Key::S},
        .modifiers = static_cast<uint32_t>(Mod::LCtrl)
    });

    auto quit_action = Action("Quit");
    quit_action.set_action(KeyChord{
        .keys = {Key::Q}
    });

    input.register_action(save_action);
    input.register_action(quit_action);

    while(1) {
        input.update();

        if (input.is_key_pressed(Key::A)) {
            std::cout << "Key A is pressed" << std::endl;
        }

        if (input.is_key_just_pressed(Key::Space)) {
            std::cout << "Key Space is just pressed" << std::endl;
        }

        if (input.is_action_pressed("Quit")) {
            std::cout << "Action Quit is pressed, exiting..." << std::endl;
            break;
        }

        if (input.is_action_pressed("Save")) {
            std::cout << "Action Save is pressed" << std::endl;
        }
    }

    return 0;
}