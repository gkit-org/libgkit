#include "graphic/backend/opengl/VertexArray.hpp"

#include "gkit/math/matrix4.hpp"
#include "gkit/math/vector4.hpp"
#include "graphic/backend/opengl/VertexBuffer.hpp"

#include <utility>

#include <glad/gl.h>

namespace {
    constexpr auto to_gl_enum(VertexElementType type) -> GLenum {
        switch (type) {
        case VertexElementType::Float:
            return GL_FLOAT;
        case VertexElementType::Uint32:
            return GL_UNSIGNED_INT;
        case VertexElementType::Uint8:
            return GL_UNSIGNED_BYTE;
        }
        return GL_FLOAT;
    }
} // namespace

namespace gkit::graphic::opengl {

    VertexArray::VertexArray() {
        glGenVertexArrays(1, &this->renderer_id);
    }

    VertexArray::VertexArray(VertexArray&& other) noexcept :
        graphic::VertexArray(std::move(other)), renderer_id(other.renderer_id), attrib_index(other.attrib_index) {
        other.renderer_id = 0;
    }

    auto VertexArray::operator=(VertexArray&& other) noexcept -> VertexArray& {
        if (this != &other) {
            if (this->renderer_id != 0) {
                glDeleteVertexArrays(1, &this->renderer_id);
            }
            graphic::VertexArray::operator=(std::move(other));
            this->renderer_id  = other.renderer_id;
            this->attrib_index = other.attrib_index;
            other.renderer_id  = 0;
        }
        return *this;
    }

    VertexArray::~VertexArray() {
        if (this->renderer_id != 0) {
            glDeleteVertexArrays(1, &this->renderer_id);
            this->renderer_id = 0;
        }
    }

    auto VertexArray::add_buffer(const graphic::VertexBuffer& vb, const graphic::VertexBufferLayout& layout) -> void {
        bind();
        vb.bind();
        const auto& elements = layout.get_elements();
        size_t offset        = 0;
        for (int i = 0; i < elements.size(); i++) {
            const auto& element = elements[i];
            glEnableVertexAttribArray(this->attrib_index);
            //Specify the reading rules for the incoming data, the meaning of the arrays (coordinates, textures, etc.)
            glVertexAttribPointer(this->attrib_index,
                                  element.count,
                                  to_gl_enum(element.type),
                                  element.normalized,
                                  layout.get_stride(),
                                  reinterpret_cast<const void*>(offset)); // NOLINT(performance-no-int-to-ptr)
            offset += element.count * graphic::VertexBufferElement::get_size_of_type(element.type);
            this->attrib_index++;
        }
    }

    auto VertexArray::add_instance_buffer(const graphic::VertexBuffer& vb) -> void {
        bind();
        vb.bind();

        size_t vec4_size = sizeof(gkit::math::Vector4);

        for (uint32_t i = 0; i < 4; i++) {
            glEnableVertexAttribArray(this->attrib_index);
            glVertexAttribPointer(this->attrib_index,
                                  4,
                                  GL_FLOAT,
                                  GL_FALSE,
                                  sizeof(gkit::math::Matrix4),
                                  reinterpret_cast<void*>(i * vec4_size) // NOLINT(performance-no-int-to-ptr)
            );
            glVertexAttribDivisor(this->attrib_index, 1);
            this->attrib_index++;
        }
    }

    auto VertexArray::bind() const -> void {
        glBindVertexArray(this->renderer_id);
    }

    auto VertexArray::unbind() const -> void {
        glBindVertexArray(0);
    }

} // namespace gkit::graphic::opengl
