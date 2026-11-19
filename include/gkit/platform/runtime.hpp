#pragma once

#include <cstdint>
#include <filesystem>
#include <string>
#include <variant>
#include <vector>

namespace gkit::platform {
    using WindowId = std::uint32_t;

    /** @brief Subsystems requested by one run. A desktop game needs only video and events. */
    struct RuntimeOptions {
        bool events = true, video = false, audio = false, gamepad = false, joystick = false;
    };

    /** @brief Window notifications. Resize describes logical units, PixelResize describes framebuffer pixels. */
    enum class WindowChange : std::uint8_t {
        Close,
        Resize,
        PixelResize,
        FocusGained,
        FocusLost,
        Minimized,
        Restored
    };

    /** @brief Application-wide quit request. */
    struct QuitEvent {};

    /** @brief Window-specific notification carrying the native window identity. */
    struct WindowEvent {
        WindowId window;
        WindowChange change;
    };

    /** @brief Physical keyboard scancode and modifier bits with the same encoding used by gkit input. */
    struct KeyEvent {
        WindowId window;
        std::uint32_t scancode, modifiers;
        bool down;
    };

    /** @brief Press or release of a mouse button in the indicated window. */
    struct MouseButtonEvent {
        WindowId window;
        std::uint8_t button;
        bool down;
    };

    /** @brief Relative horizontal and vertical mouse motion in window coordinates. */
    struct MotionEvent {
        WindowId window;
        float x, y;
    };

    /** @brief Scroll distance on both axes in native wheel units. */
    struct WheelEvent {
        WindowId window;
        float x, y;
    };

    /** @brief Press or release of a gamepad button, identified by device instance and button index. */
    struct GamepadButtonEvent {
        std::uint32_t device;
        std::uint8_t button;
        bool down;
    };

    /** @brief Owned event values in native queue order. Motion is relative to the preceding mouse position. */
    using Event =
        std::variant<QuitEvent, WindowEvent, KeyEvent, MouseButtonEvent, MotionEvent, WheelEvent, GamepadButtonEvent>;

    /** @brief Acquire and release SDL subsystem references on the main thread. May coexist with a host's SDL session. */
    class Runtime {
    public:
        /**
         * @brief Acquire subsystem references without taking ownership of any external initialization.
         * @throws std::runtime_error If SDL initialization fails.
         * @throws std::logic_error When called outside the platform main thread.
         */
        explicit Runtime(RuntimeOptions options = {});
        /** @brief Compatibility entry for existing Processer SDL flags. */
        explicit Runtime(std::uint32_t native_flags);
        ~Runtime();
        Runtime(const Runtime&)                    = delete;
        auto operator=(const Runtime&) -> Runtime& = delete;

        /** @brief Whether this session requested native video support. */
        [[nodiscard]] auto has_video() const noexcept -> bool;

        /** @brief Whether the acquired subsystems include platform events, explicitly or by dependency. */
        [[nodiscard]] auto has_events() const noexcept -> bool;

        /** @brief Main-thread event pump. The caller owns the returned batch. */
        [[nodiscard]] auto poll_events() const -> std::vector<Event>;

    private:
        std::uint32_t flags;
    };

    /** @brief Throw std::logic_error when a main-thread operation would be invalid. */
    auto require_main_thread() -> void;

    /** @brief Return the application resource base as a native filesystem path. Throws on platform failure. */
    auto base_path() -> std::filesystem::path;

    /** @brief Locate and, when necessary, create the per-user directory for this organization and application. */
    auto user_data_path(const std::string& organization, const std::string& application) -> std::filesystem::path;
} // namespace gkit::platform
