#pragma once

#include "gkit/core/node.hpp"
#include "gkit/graphic/render/viewport_rect.hpp"

#include <mutex>

namespace gkit::graphic {
    /** @brief Render descendants in local logical pixels. A nested viewport starts a new coordinate system.
     * The default fills its parent. Content is clipped to all enclosing viewports in tree order. */
    class Viewport : public core::Node {
    public:
        /** @brief Construct a viewport that fills its parent. */
        Viewport() = default;
        /** @brief Position the view relative to its parent's top-left corner. Width and height must be positive. */
        auto set_rect(int x, int y, int width, int height) -> void {
            if (width <= 0 || height <= 0) throw std::invalid_argument("viewport dimensions must be positive");
            std::lock_guard lock(mutex);
            rect = {x, y, width, height};
        }
        /** @brief Read the logical rectangle, or nullopt when the view fills its parent. */
        auto get_rect() const -> std::optional<ViewportRect> {
            std::lock_guard lock(mutex);
            return rect;
        }

    private:
        mutable std::mutex mutex;
        std::optional<ViewportRect> rect;
    };
} // namespace gkit::graphic
