#include "graphic/backend/opengl/vertex_buffer.hpp"

#include <glad/gl.h>

namespace gkit::graphic::opengl {

    VertexBuffer::VertexBuffer(const std::shared_ptr<const Context>& context,
                               const void* data,
                               uint32_t size,
                               bool dynamic) : ContextResource(context) {
        glGenBuffers(1, &this->renderer_id);
        glBindBuffer(GL_ARRAY_BUFFER, this->renderer_id);
        if (!dynamic) {
            glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
        } else {
            glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
        }
        this->size    = size;
        this->dynamic = dynamic;
    }

    VertexBuffer::~VertexBuffer() {
        if (renderer_id != 0 && has_current_context()) {
            glDeleteBuffers(1, &this->renderer_id);
            this->renderer_id = 0;
        }
    }

    auto VertexBuffer::bind() const -> void {
        require_context();
        glBindBuffer(GL_ARRAY_BUFFER, this->renderer_id);
    }

    auto VertexBuffer::unbind() const -> void {
        require_context();
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    auto VertexBuffer::update_data(const void* data, uint32_t size) -> void {
        require_context();
        glBindBuffer(GL_ARRAY_BUFFER, this->renderer_id);
        if (size == this->size) {
            glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
        } else {
            glBufferData(GL_ARRAY_BUFFER, size, data, GL_DYNAMIC_DRAW);
            this->size = size;
        }
    }

    auto VertexBuffer::update_sub_data(uint32_t offset, const void* data, uint32_t size) -> void {
        require_context();
        glBindBuffer(GL_ARRAY_BUFFER, this->renderer_id);
        glBufferSubData(GL_ARRAY_BUFFER, offset, size, data);
    }

} // namespace gkit::graphic::opengl
