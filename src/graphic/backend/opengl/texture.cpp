#include "graphic/backend/opengl/texture.hpp"

#include <glad/gl.h>

namespace gkit::graphic::opengl {
    Texture::Texture(const std::shared_ptr<const Context>& context,
                     int width,
                     int height,
                     const void* pixels,
                     int pitch) : ContextResource(context), width(width), height(height) {
        GLint max_size = 0;
        glGetIntegerv(GL_MAX_TEXTURE_SIZE, &max_size);
        if (width > max_size || height > max_size) throw std::invalid_argument("texture exceeds GL_MAX_TEXTURE_SIZE");

        GLint previous = 0;
        glGetIntegerv(GL_TEXTURE_BINDING_2D, &previous);
        glGenTextures(1, &renderer_id);
        glBindTexture(GL_TEXTURE_2D, renderer_id);
        GLint buffer, alignment, row_length, skip_pixels, skip_rows;
        glGetIntegerv(GL_PIXEL_UNPACK_BUFFER_BINDING, &buffer);
        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
        glGetIntegerv(GL_UNPACK_ALIGNMENT, &alignment);
        glGetIntegerv(GL_UNPACK_ROW_LENGTH, &row_length);
        glGetIntegerv(GL_UNPACK_SKIP_PIXELS, &skip_pixels);
        glGetIntegerv(GL_UNPACK_SKIP_ROWS, &skip_rows);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glPixelStorei(GL_UNPACK_ROW_LENGTH, pitch / 4);
        glPixelStorei(GL_UNPACK_SKIP_PIXELS, 0);
        glPixelStorei(GL_UNPACK_SKIP_ROWS, 0);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
        glPixelStorei(GL_UNPACK_ALIGNMENT, alignment);
        glPixelStorei(GL_UNPACK_ROW_LENGTH, row_length);
        glPixelStorei(GL_UNPACK_SKIP_PIXELS, skip_pixels);
        glPixelStorei(GL_UNPACK_SKIP_ROWS, skip_rows);
        glBindBuffer(GL_PIXEL_UNPACK_BUFFER, buffer);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, pixels ? GL_NEAREST : GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, pixels ? GL_NEAREST : GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glBindTexture(GL_TEXTURE_2D, previous);
    }

    Texture::~Texture() {
        if (renderer_id != 0 && has_current_context()) glDeleteTextures(1, &renderer_id);
    }

    auto Texture::bind(unsigned int slot) const -> void {
        require_context();
        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(GL_TEXTURE_2D, renderer_id);
    }

    auto Texture::unbind() const -> void {
        require_context();
        glBindTexture(GL_TEXTURE_2D, 0);
    }
} // namespace gkit::graphic::opengl
