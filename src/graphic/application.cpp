#include "gkit/graphic/application.hpp"

#include "graphic/backend/opengl/context.hpp"
#include "platform/window_access.hpp"

#include <type_traits>
#include <utility>

#include <glad/gl.h>

namespace gkit::graphic {
    namespace {
        struct DestroyContext {
            auto operator()(SDL_GLContext context) const noexcept -> void { SDL_GL_DestroyContext(context); }
        };
        using ContextHandle = std::unique_ptr<std::remove_pointer_t<SDL_GLContext>, DestroyContext>;

        auto create_context(SDL_Window* window) -> ContextHandle {
            platform::require_main_thread();
            if (!(SDL_GetWindowFlags(window) & SDL_WINDOW_OPENGL))
                throw std::invalid_argument("an application requires an OpenGL window");
            if (SDL_GL_GetCurrentContext()) throw std::logic_error("an OpenGL context is already current");
            ContextHandle context(SDL_GL_CreateContext(window));
            if (!context) throw std::runtime_error(std::string("SDL_GL_CreateContext: ") + SDL_GetError());
            if (gladLoadGL(SDL_GL_GetProcAddress) < GLAD_MAKE_VERSION(4, 1))
                throw std::runtime_error("the application requires OpenGL 4.1");
            return context;
        }
    } // namespace

    struct Application::Session {
        SDL_Window* native;
        ContextHandle context;
        Renderer renderer;

        explicit Session(platform::Window& window) :
            native(platform::detail::WindowAccess::native(window)), context(create_context(native)),
            renderer(std::make_shared<opengl::Context>(opengl::Context{context.get()})) {}
    };

    Application::Application(platform::WindowOptions options) {
        if (options.surface != platform::Surface::OpenGL)
            throw std::invalid_argument("an application requires an OpenGL window");
        set_platform_options({.video = true}).set_window_options(std::move(options));
    }

    Application::~Application() = default;

    auto Application::renderer() -> Renderer& {
        platform::require_main_thread();
        if (!session) throw std::logic_error("the application has no active renderer");
        session->renderer.require_context();
        return session->renderer;
    }
    auto Application::start_run_services() -> void {
        session = std::make_unique<Session>(window());
    }
    auto Application::finish_frame() -> void {
        const auto state = window().state();
        if (!state.drawable()) return;
        auto& active_renderer = renderer();
        active_renderer.begin_frame();
        try {
            render();
            active_renderer.finish_frame({0, 0, state.pixel_width, state.pixel_height});
            if (!SDL_GL_SwapWindow(session->native))
                throw std::runtime_error(std::string("SDL_GL_SwapWindow: ") + SDL_GetError());
        } catch (...) {
            active_renderer.discard_frame();
            throw;
        }
    }
    auto Application::stop_run_services() noexcept -> void {
        session.reset();
    }
} // namespace gkit::graphic
