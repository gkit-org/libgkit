#include "gkit/platform/runtime.hpp"

#include "platform/sdl_event_dispatcher.hpp"

#include <SDL3/SDL.h>

#include <memory>
#include <stdexcept>
#include <string_view>

namespace gkit::platform {
    namespace {
        auto filesystem_path(std::string_view utf8) -> std::filesystem::path {
            return std::u8string(utf8.begin(), utf8.end());
        }
        auto native_flags(RuntimeOptions o) -> SDL_InitFlags {
            return (o.events ? SDL_INIT_EVENTS : 0u) | (o.video ? SDL_INIT_VIDEO : 0u) |
                   (o.audio ? SDL_INIT_AUDIO : 0u) | (o.gamepad ? SDL_INIT_GAMEPAD : 0u) |
                   (o.joystick ? SDL_INIT_JOYSTICK : 0u);
        }
        auto append_event(const SDL_Event& event, std::vector<Event>& out) -> void {
            switch (event.type) {
            case SDL_EVENT_QUIT:
                out.emplace_back(QuitEvent{});
                break;
            case SDL_EVENT_KEY_DOWN:
            case SDL_EVENT_KEY_UP:
                out.emplace_back(KeyEvent{event.key.windowID,
                                          static_cast<std::uint32_t>(event.key.scancode),
                                          event.key.mod,
                                          event.type == SDL_EVENT_KEY_DOWN});
                break;
            case SDL_EVENT_MOUSE_BUTTON_DOWN:
            case SDL_EVENT_MOUSE_BUTTON_UP:
                out.emplace_back(MouseButtonEvent{
                    event.button.windowID, event.button.button, event.type == SDL_EVENT_MOUSE_BUTTON_DOWN});
                break;
            case SDL_EVENT_MOUSE_MOTION:
                out.emplace_back(MotionEvent{event.motion.windowID, event.motion.xrel, event.motion.yrel});
                break;
            case SDL_EVENT_MOUSE_WHEEL:
                out.emplace_back(WheelEvent{event.wheel.windowID, event.wheel.x, event.wheel.y});
                break;
            case SDL_EVENT_GAMEPAD_BUTTON_DOWN:
            case SDL_EVENT_GAMEPAD_BUTTON_UP:
                out.emplace_back(GamepadButtonEvent{
                    event.gbutton.which, event.gbutton.button, event.type == SDL_EVENT_GAMEPAD_BUTTON_DOWN});
                break;
            case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
                out.emplace_back(WindowEvent{event.window.windowID, WindowChange::Close});
                break;
            case SDL_EVENT_WINDOW_RESIZED:
                out.emplace_back(WindowEvent{event.window.windowID, WindowChange::Resize});
                break;
            case SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED:
                out.emplace_back(WindowEvent{event.window.windowID, WindowChange::PixelResize});
                break;
            case SDL_EVENT_WINDOW_FOCUS_GAINED:
                out.emplace_back(WindowEvent{event.window.windowID, WindowChange::FocusGained});
                break;
            case SDL_EVENT_WINDOW_FOCUS_LOST:
                out.emplace_back(WindowEvent{event.window.windowID, WindowChange::FocusLost});
                break;
            case SDL_EVENT_WINDOW_MINIMIZED:
                out.emplace_back(WindowEvent{event.window.windowID, WindowChange::Minimized});
                break;
            case SDL_EVENT_WINDOW_RESTORED:
                out.emplace_back(WindowEvent{event.window.windowID, WindowChange::Restored});
                break;
            default:
                break;
            }
        }
    } // namespace
    auto require_main_thread() -> void {
        if (!SDL_IsMainThread()) throw std::logic_error("platform operation requires the main thread");
    }
    Runtime::Runtime(RuntimeOptions options) : Runtime(native_flags(options)) {}
    Runtime::Runtime(std::uint32_t native) : flags(native) {
        require_main_thread();
        if (!SDL_InitSubSystem(flags)) throw std::runtime_error(std::string("SDL_InitSubSystem: ") + SDL_GetError());
    }
    Runtime::~Runtime() {
        SDL_QuitSubSystem(flags);
    }
    auto Runtime::has_video() const noexcept -> bool {
        return (flags & SDL_INIT_VIDEO) != 0;
    }
    auto Runtime::has_events() const noexcept -> bool {
        return (flags & (SDL_INIT_EVENTS | SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_JOYSTICK | SDL_INIT_GAMEPAD |
                         SDL_INIT_CAMERA)) != 0;
    }
    auto Runtime::poll_events() const -> std::vector<Event> {
        require_main_thread();
        std::vector<Event> events;
        if (has_events()) {
            detail::SDLEventDispatcher::instance().dispatch_events([&](const SDL_Event& event) {
                append_event(event, events);
            });
        }
        return events;
    }
    auto base_path() -> std::filesystem::path {
        const auto* path = SDL_GetBasePath();
        if (!path) throw std::runtime_error(std::string("SDL_GetBasePath: ") + SDL_GetError());
        return filesystem_path(path);
    }
    auto user_data_path(const std::string& organization, const std::string& application) -> std::filesystem::path {
        if (organization.empty() || application.empty())
            throw std::invalid_argument("missing organization or application");
        std::unique_ptr<char, decltype(&SDL_free)> path(SDL_GetPrefPath(organization.c_str(), application.c_str()),
                                                        SDL_free);
        if (!path) throw std::runtime_error(std::string("SDL_GetPrefPath: ") + SDL_GetError());
        return filesystem_path(path.get());
    }
} // namespace gkit::platform
