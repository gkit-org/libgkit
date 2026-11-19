#include "graphic/backend/opengl/config.hpp"

#include <glad/gl.h>

namespace gkit::graphic::opengl::viewport {

    auto set_viewport(int x, int y, int width, int height) -> void {
        glViewport(x, y, width, height);
    }


    auto set_viewport(int width, int height) -> void {
        glViewport(0, 0, width, height);
    }
} // namespace gkit::graphic::opengl::viewport
