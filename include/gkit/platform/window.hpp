#pragma once

#include "gkit/platform/runtime.hpp"

#include <memory>

namespace gkit::platform {
    namespace detail {
        struct WindowAccess;
    }

    /** @brief Native surface capability selected before window creation. */
    enum class Surface {
        Plain,
        OpenGL
    };

    /** @brief Initial logical dimensions and surface requirements. Sizes must be positive. */
    struct WindowOptions {
        std::string title = "gkit";
        int width = 800, height = 600;
        bool resizable  = true;
        Surface surface = Surface::OpenGL;
    };

    /** @brief Copyable state. Logical dimensions are in window coordinates, pixel dimensions describe the drawable. */
    struct WindowState {
        WindowId id{};
        int width{}, height{}, pixel_width{}, pixel_height{};
        bool focused{}, minimized{};
        auto drawable() const noexcept -> bool { return !minimized && pixel_width > 0 && pixel_height > 0; }
    };

    /** @brief Main-thread window. Keeps its video subsystem alive until the native window is destroyed. */
    class Window {
    public:
        explicit Window(const WindowOptions& options);
        ~Window();
        Window(const Window&) = delete;

        auto operator=(const Window&) -> Window& = delete;
        /** @brief Read a synchronized copy, including from a worker during a frame. */
        auto state() const -> WindowState;
        /** @brief Change the native title on the main thread. Throws on failure. */
        auto set_title(const std::string& title) -> void;
        /** @brief Request a size in logical units. The window manager confirms the change with a resize event. */
        auto set_size(int width, int height) -> void;
        /** @brief Refresh geometry and focus after a window event. Main thread only. */
        auto refresh() -> void;

    private:
        friend struct detail::WindowAccess;
        struct Impl;
        std::unique_ptr<Impl> impl;
    };
} // namespace gkit::platform
