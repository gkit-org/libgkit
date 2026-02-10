#include "../include/application.hpp"
#include <SDL3/SDL_error.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_log.h>

gkit::Application::Application() noexcept  {
    SDL_InitFlags flags = SDL_INIT_AUDIO |
                          SDL_INIT_EVENTS |
                          SDL_INIT_GAMEPAD |
                          SDL_INIT_JOYSTICK |
                          SDL_INIT_VIDEO ;

    if (!SDL_Init(flags)) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "filed to initialize SDL: %s\n", SDL_GetError());
        return;
    }
}

gkit::Application::~Application() noexcept {
    SDL_Quit();
}