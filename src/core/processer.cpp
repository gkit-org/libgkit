#include "gkit/core/processer.hpp"
#include "gkit/core/scene/unit.hpp"
#include <SDL3/SDL_error.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_log.h>

gkit::Processer::Processer() noexcept : root() {
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

gkit::Processer::~Processer() noexcept {
    SDL_Quit();
}


auto gkit::Processer::set_root(std::unique_ptr<scene::Unit>&& root_ptr) noexcept -> void {
    if (root_ptr == nullptr) return;
    this->root = std::move(root_ptr);
}


auto gkit::Processer::run() -> void {
    this->root->ready_handler();
    this->running.store(true);
    while (this->running.load()) {
        this->root->process_handler();
    }
}


auto gkit::Processer::stop() -> void {
    this->running.store(false);
}
