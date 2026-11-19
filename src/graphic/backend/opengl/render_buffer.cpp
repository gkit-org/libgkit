#include "graphic/backend/opengl/render_buffer.hpp"

#include <glad/gl.h>

namespace gkit::graphic::opengl {

    RenderBuffer::RenderBuffer(const std::shared_ptr<const Context>& context, int width, int height) :
        ContextResource(context), buffer_width(width), buffer_height(height) {
        GLint max_size = 0;
        glGetIntegerv(GL_MAX_RENDERBUFFER_SIZE, &max_size);
        if (width > max_size || height > max_size)
            throw std::invalid_argument("depth-stencil attachment exceeds GL_MAX_RENDERBUFFER_SIZE");
        glGenRenderbuffers(1, &this->renderer_id);
        glBindRenderbuffer(GL_RENDERBUFFER, this->renderer_id);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
        unbind();
    }


    RenderBuffer::~RenderBuffer() {
        if (renderer_id != 0 && has_current_context()) {
            glDeleteRenderbuffers(1, &this->renderer_id);
            this->renderer_id = 0;
        }
    }


    auto RenderBuffer::bind() const -> void {
        require_context();
        glBindRenderbuffer(GL_RENDERBUFFER, this->renderer_id);
    }


    auto RenderBuffer::unbind() const -> void {
        require_context();
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
    }

} // namespace gkit::graphic::opengl
