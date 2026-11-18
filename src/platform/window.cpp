#include "platform/window_access.hpp"
#include "platform/window_close_policy.hpp"

#include <SDL3/SDL.h>

#include <mutex>
#include <stdexcept>

namespace gkit::platform {
    namespace {
        auto check(bool ok, const char* operation) -> void {
            if (!ok) throw std::runtime_error(std::string(operation) + ": " + SDL_GetError());
        }
    } // namespace
    detail::KeepRunningOnWindowClose::KeepRunningOnWindowClose() {
        require_main_thread();
        if (!SDL_GetHintBoolean(SDL_HINT_QUIT_ON_LAST_WINDOW_CLOSE, true)) return;
        if (const char* value = SDL_GetHint(SDL_HINT_QUIT_ON_LAST_WINDOW_CLOSE)) previous = value;
        if (!SDL_SetHint(SDL_HINT_QUIT_ON_LAST_WINDOW_CLOSE, "0")) {
            override_priority = true;
            check(SDL_SetHintWithPriority(SDL_HINT_QUIT_ON_LAST_WINDOW_CLOSE, "0", SDL_HINT_OVERRIDE), "close policy");
        }
        changed = true;
    }
    detail::KeepRunningOnWindowClose::~KeepRunningOnWindowClose() {
        if (!changed) return;
        SDL_ResetHint(SDL_HINT_QUIT_ON_LAST_WINDOW_CLOSE);
        const char* restored = SDL_GetHint(SDL_HINT_QUIT_ON_LAST_WINDOW_CLOSE);
        if (previous && (!restored || *previous != restored))
            SDL_SetHintWithPriority(SDL_HINT_QUIT_ON_LAST_WINDOW_CLOSE,
                                    previous->c_str(),
                                    override_priority ? SDL_HINT_OVERRIDE : SDL_HINT_NORMAL);
    }
    struct Window::Impl {
        Runtime video{RuntimeOptions{.video = true}};
        std::unique_ptr<SDL_Window, decltype(&SDL_DestroyWindow)> native{nullptr, SDL_DestroyWindow};
        std::mutex mutex;
        WindowState cached;
    };
    auto detail::WindowAccess::native(Window& window) -> SDL_Window* {
        return window.impl->native.get();
    }
    Window::Window(const WindowOptions& options) {
        require_main_thread();
        if (options.width <= 0 || options.height <= 0)
            throw std::invalid_argument("window dimensions must be positive");
        impl                  = std::make_unique<Impl>();
        SDL_WindowFlags flags = SDL_WINDOW_HIGH_PIXEL_DENSITY;
        if (options.resizable) flags |= SDL_WINDOW_RESIZABLE;
        if (options.surface == Surface::OpenGL) {
            flags |= SDL_WINDOW_OPENGL;
            check(SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4), "OpenGL major version");
            check(SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1), "OpenGL minor version");
            check(SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE), "OpenGL core profile");
        }
        impl->native.reset(SDL_CreateWindow(options.title.c_str(), options.width, options.height, flags));
        check(bool(impl->native), "SDL_CreateWindow");
        refresh();
    }
    Window::~Window() = default;
    auto Window::state() const -> WindowState {
        std::lock_guard lock(impl->mutex);
        return impl->cached;
    }
    auto Window::refresh() -> void {
        require_main_thread();
        auto* window = impl->native.get();
        WindowState next;
        next.id = SDL_GetWindowID(window);
        check(SDL_GetWindowSize(window, &next.width, &next.height), "SDL_GetWindowSize");
        check(SDL_GetWindowSizeInPixels(window, &next.pixel_width, &next.pixel_height), "SDL_GetWindowSizeInPixels");
        auto flags     = SDL_GetWindowFlags(window);
        next.focused   = flags & SDL_WINDOW_INPUT_FOCUS;
        next.minimized = flags & SDL_WINDOW_MINIMIZED;
        std::lock_guard lock(impl->mutex);
        impl->cached = next;
    }
    auto Window::set_title(const std::string& title) -> void {
        require_main_thread();
        check(SDL_SetWindowTitle(impl->native.get(), title.c_str()), "SDL_SetWindowTitle");
    }
    auto Window::set_size(int width, int height) -> void {
        require_main_thread();
        if (width <= 0 || height <= 0) throw std::invalid_argument("window dimensions must be positive");
        check(SDL_SetWindowSize(impl->native.get(), width, height), "SDL_SetWindowSize");
    }
} // namespace gkit::platform
