#pragma once

#include "gkit/graphic/viewport.hpp"
#include "gkit/platform/window.hpp"

#include <functional>
#include <mutex>

namespace gkit::graphic {
    class Application;

    /** @brief A viewport that can serve as the application-managed main window.
     * As the root viewport it always uses the native logical window size.
     * The application opens a native window for its designated main window node.
     * Settings apply the next time it opens. */
    class Window final : public Viewport {
    public:
        /** @brief Construct a closed node with the default application window options. */
        Window() = default;

        /** @brief Set the title used when opening the window. */
        auto set_title(std::string title) -> Window& {
            std::lock_guard lock(mutex);
            options.title = std::move(title);
            return *this;
        }

        /** @brief Set positive initial dimensions in logical pixels. */
        auto set_size(int width, int height) -> Window& {
            if (width <= 0 || height <= 0) throw std::invalid_argument("window dimensions must be positive");
            std::lock_guard lock(mutex);
            options.width  = width;
            options.height = height;
            return *this;
        }

        /** @brief Allow or prevent resizing of the next native window. */
        auto set_resizable(bool value) -> Window& {
            std::lock_guard lock(mutex);
            options.resizable = value;
            return *this;
        }

        /** @brief Read confirmed native state from any thread. Empty while closed. */
        auto state() const -> std::optional<platform::WindowState> {
            std::lock_guard lock(mutex);
            return current;
        }

    private:
        friend class Application;

        auto notify_tree_change(bool entering) -> void;
        auto enter_tree() -> void override;
        auto exit_tree() -> void override;

        core::ObjectId application_root;
        std::function<void(Window&, bool entering)> tree_changed;
        mutable std::mutex mutex;
        platform::WindowOptions options;
        std::optional<platform::WindowState> current;
    };
} // namespace gkit::graphic
