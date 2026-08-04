#include "gkit/core/processer.hpp"

#include "gkit/scene/unit.hpp"

#include <SDL3/SDL_error.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_log.h>

gkit::Processer::Processer() noexcept : root(gkit::core::UniqueObject::create<gkit::scene::Unit>()) {
    SDL_InitFlags flags = SDL_INIT_AUDIO | SDL_INIT_EVENTS | SDL_INIT_GAMEPAD | SDL_INIT_JOYSTICK | SDL_INIT_VIDEO;

    if (!SDL_Init(flags)) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "filed to initialize SDL: %s\n", SDL_GetError());
        return;
    }
}

gkit::Processer::~Processer() noexcept {
    SDL_Quit();
}

auto gkit::Processer::set_root(core::UniqueObject&& root_p) noexcept -> void {
    auto root_ptr = root_p.get();
    if (!root_ptr) return;
    this->root = std::move(root_p);
}

auto gkit::Processer::run() -> void {
    auto root_ptr = dynamic_cast<scene::Unit*>(this->root.get());
    root_ptr->ready_handler();
    this->running.store(true);
    while (this->running.load()) {
        root_ptr->process_handler();
    }
}

auto gkit::Processer::stop() -> void {
    this->running.store(false);
}
