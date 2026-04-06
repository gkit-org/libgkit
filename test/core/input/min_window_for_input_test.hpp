#pragma once

#include <SDL3/SDL.h>
#include <iostream>

namespace gkit::test {
    // A window for event system to work 
    struct Window {
        Window() {
            SDL_Init(SDL_INIT_VIDEO);

            win_ptr = SDL_CreateWindow("Window for input test", 640, 480, SDL_WINDOW_RESIZABLE);
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
}