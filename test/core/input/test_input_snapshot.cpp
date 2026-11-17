#include "core/misc/sdl_event_dispatcher.hpp"
#include "gkit/core/input/input.hpp"
#include "test_utils.hpp"

#include <thread>

auto test_snapshot() -> bool {
    gkit::test::assert_if(SDL_Init(SDL_INIT_EVENTS), "SDL event initialization");
    auto& input = gkit::Input::instance();
    input.initialize();
    auto& dispatcher = gkit::core::SDLEventDispatcher::instance();
    SDL_Event event{};
    event.type        = SDL_EVENT_MOUSE_MOTION;
    event.motion.xrel = 3;
    event.motion.yrel = -2;
    SDL_PushEvent(&event);
    SDL_PushEvent(&event);
    event              = {};
    event.type         = SDL_EVENT_KEY_DOWN;
    event.key.scancode = SDL_SCANCODE_A;
    SDL_PushEvent(&event);
    event.type = SDL_EVENT_KEY_UP;
    SDL_PushEvent(&event);
    int calls   = 0;
    auto first  = dispatcher.register_event_handler(SDL_EVENT_USER, [&](const auto&) {
        ++calls;
    });
    auto second = dispatcher.register_event_handler(SDL_EVENT_USER, [&](const auto&) {
        calls += 10;
    });
    dispatcher.disconnect(first);
    event      = {};
    event.type = SDL_EVENT_USER;
    SDL_PushEvent(&event);
    dispatcher.dispatch_events();
    auto frame = input.snapshot();
    gkit::test::assert_if(frame->motion.x == 6 && frame->motion.y == -4, "accumulate motion");
    auto key = static_cast<gkit::input::Key>(SDL_SCANCODE_A);
    gkit::test::assert_if(frame->pressed_keys.contains(key) && frame->released_keys.contains(key), "same-frame edges");
    gkit::test::assert_if(calls == 10, "disconnect must preserve other subscribers");
    dispatcher.disconnect(second);
    gkit::core::SDLEventDispatcher::Token self = 0;
    self                                       = dispatcher.register_handler_before_frame([&] {
        dispatcher.disconnect(self);
    });
    std::thread reader([&] {
        for (int i = 0; i < 1000; ++i) gkit::test::assert_if(input.snapshot() != nullptr, "published snapshot");
    });
    dispatcher.dispatch_events();
    reader.join();
    gkit::test::assert_if(input.snapshot()->motion.x == 0, "reset frame delta");
    gkit::test::assert_if(frame->motion.x == 6, "old snapshot is immutable");
    SDL_Quit();
    return true;
}
auto main() -> int {
    gkit::test::TestRunner().add_test_func(test_snapshot).run();
}
