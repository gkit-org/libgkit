#include "graphic/backend/opengl/texture.hpp"

#include <glad/gl.h>

namespace gkit::graphic::opengl {
    Texture::Texture(const std::shared_ptr<const Context>& context, int width, int height) :
        ContextResource(context), width(width), height(height) {
        GLint max_size = 0;
        glGetIntegerv(GL_MAX_TEXTURE_SIZE, &max_size);
        if (width > max_size || height > max_size)
            throw std::invalid_argument("color attachment exceeds GL_MAX_TEXTURE_SIZE");

        GLint previous = 0;
        glGetIntegerv(GL_TEXTURE_BINDING_2D, &previous);
        glGenTextures(1, &renderer_id);
        glBindTexture(GL_TEXTURE_2D, renderer_id);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
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
