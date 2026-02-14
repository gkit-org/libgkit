#include "graphic/window.hpp"
#include <SDL3/SDL_error.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_log.h>
#include <SDL3/SDL_video.h>

gkit::graphic::Window::Window(std::string title, int width, int height) noexcept 
    : gkit::scene::Unit()
    , window_ptr(SDL_CreateWindow(title.c_str(), width, height, SDL_WINDOW_RESIZABLE), SDL_DestroyWindow)
    , renderer_ptr(SDL_CreateRenderer(this->window_ptr.get(), nullptr), SDL_DestroyRenderer) {
    
    if (!SDL_InitSubSystem(SDL_INIT_VIDEO)) {
        SDL_Log("SDL_INIT_VIDEO initialize failed, error: %s", SDL_GetError());
    }
}

gkit::graphic::Window::Window(std::string title, int width, int height, std::vector<SDL_WindowFlags> flags) noexcept 
    : gkit::graphic::Window::Window(title, width, height) {
    SDL_WindowFlags win_flags = 0x00ul;
    for (auto flag : flags) {
        win_flags |= flag;
    }

    this->window_ptr.reset(SDL_CreateWindow(title.c_str(), width, height, win_flags));
}

void gkit::graphic::Window::hide() noexcept {
    SDL_HideWindow(this->window_ptr.get());
}

void gkit::graphic::Window::show() noexcept {
    SDL_ShowWindow(this->window_ptr.get());
}