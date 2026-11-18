#include "gkit/core/input/input.hpp"
#include "gkit/core/processer.hpp"
#include "gkit/platform/window.hpp"
#include "test_utils.hpp"

#include <SDL3/SDL.h>

#include <thread>

namespace {
    using gkit::test::assert_if;
    class WindowApp final : public gkit::Processer {
    public:
        gkit::platform::WindowId id = 0;
        int stops                   = 0;
        bool push_key               = true;
        std::optional<gkit::platform::Window> native;
        gkit::platform::WindowOptions options;
        auto on_start() -> void override {
            auto state = native.emplace(options).state();
            assert_if(state.width == 320 && state.height == 240 && state.drawable(), "window geometry");
            assert_if(state.id && state.id != id, "new window identity");
            id = state.id;
            assert_if(gkit::Input::instance().is_key_released(gkit::input::Key::A),
                      "no input carried over from previous run");
            SDL_Event e{};
            e.type            = SDL_EVENT_WINDOW_CLOSE_REQUESTED;
            e.window.windowID = id + 1;
            SDL_PushEvent(&e);
            e              = {};
            e.type         = SDL_EVENT_KEY_DOWN;
            e.key.windowID = id;
            e.key.scancode = SDL_SCANCODE_A;
            if (push_key) SDL_PushEvent(&e);
        }
        auto after_frame() -> void override {
            assert_if(get_frame_context().frame == 0, "global quit stops before the next update");
            assert_if(gkit::Input::instance().is_key_pressed(gkit::input::Key::A) == push_key, "per-run event batch");
            SDL_Event e{};
            e.type = SDL_EVENT_QUIT;
            SDL_PushEvent(&e);
        }
        auto on_stop() -> void override {
            if (!native) return;
            assert_if(native->state().id == id, "window alive through teardown");
            native.reset();
            ++stops;
        }
    };
    auto test_lifetime() -> bool {
        using namespace gkit::platform;
        Runtime host;
        bool rejected = false;
        std::thread other([&] {
            try {
                host.poll_events();
            } catch (const std::logic_error&) {
                rejected = true;
            }
        });
        other.join();
        assert_if(rejected, "main thread affinity");
        WindowApp app;
        app.set_initialize_sdl(false).set_platform_options({.video = true}).set_target_frame_rate(0);
        WindowOptions options{.width = -1, .surface = Surface::Plain};
        app.options = options;
        try {
            app.run();
            return false;
        } catch (const std::invalid_argument&) {
        }
        assert_if((SDL_WasInit(SDL_INIT_VIDEO) & SDL_INIT_VIDEO) == 0, "window failure releases video");
        options.width  = 320;
        options.height = 240;
        app.options    = options;
        app.run();
        app.push_key = false;
        app.run();
        assert_if(app.stops == 2 && SDL_WasInit(SDL_INIT_EVENTS), "preserve host events");
        assert_if(base_path().is_absolute(), "absolute resource base");
        return true;
    }
} // namespace
int main() {
    gkit::test::TestRunner().add_test_func(test_lifetime).run();
}
