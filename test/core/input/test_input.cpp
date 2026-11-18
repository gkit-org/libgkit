#include "gkit/core/input/input.hpp"
#include "platform/sdl_event_dispatcher.hpp"
#include "test_utils.hpp"

#include <thread>

auto test_event_batch() -> bool {
    gkit::platform::Runtime runtime;
    auto& input = gkit::Input::instance();
    input.initialize();
    auto& dispatcher = gkit::platform::detail::SDLEventDispatcher::instance();
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
    input.update(runtime.poll_events());
    const auto motion = input.get_mouse_move();
    gkit::test::assert_if(motion.x == 6 && motion.y == -4, "accumulate motion");
    auto key = static_cast<gkit::input::Key>(SDL_SCANCODE_A);
    gkit::test::assert_if(input.is_key_just_pressed(key) && input.is_key_just_released(key), "same-frame edges");
    gkit::test::assert_if(input.is_key_released(key), "key's final state");
    gkit::test::assert_if(calls == 10, "disconnect must preserve other subscribers");
    dispatcher.disconnect(second);
    gkit::platform::detail::SDLEventDispatcher::Token self = 0;
    self                                                   = dispatcher.register_handler_before_frame([&] {
        dispatcher.disconnect(self);
    });
    std::thread reader([&] {
        for (int i = 0; i < 1000; ++i) {
            const auto move = input.get_mouse_move();
            gkit::test::assert_if((move.x == 6 && move.y == -4) || (move.x == 0 && move.y == 0), "whole input batch");
        }
    });
    input.update(runtime.poll_events());
    reader.join();
    gkit::test::assert_if(input.get_mouse_move().x == 0, "reset frame delta");
    gkit::test::assert_if(!input.is_key_just_pressed(key) && !input.is_key_just_released(key), "reset frame edges");
    return true;
}
auto main() -> int {
    gkit::test::TestRunner().add_test_func(test_event_batch).run();
}
