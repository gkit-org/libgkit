#pragma once

namespace gkit::graphic {
    /** @brief Rectangle with integer extents. Draw commands take bottom-left target pixels; viewport nodes take top-left logical units. */
    struct ViewportRect {
        int x = 0, y = 0;
        int width = 0, height = 0;
    };
} // namespace gkit::graphic
