#pragma once

namespace gkit::graphic {
    /** @brief Viewport rectangle in target pixels, measured from the bottom-left corner. */
    struct Viewport {
        int x = 0, y = 0;
        int width = 0, height = 0;
    };
} // namespace gkit::graphic
