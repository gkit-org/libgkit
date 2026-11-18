#pragma once

#include "gkit/graphic/resource/vertex_array.hpp"
#include "gkit/graphic/resource/vertex_buffer.hpp"
#include "gkit/graphic/vertex_buffer_layout.hpp"
#include "graphic/backend/opengl/context.hpp"

#include <cstdint>

/**
 * @brief OpenGL backend vertex array (VAO)
 *
 * Inherits frontend `graphic::VertexArray`; maintains GL vertex attribute
 * configuration.
 */
namespace gkit::graphic::opengl {

    class VertexArray final : public graphic::VertexArray, public ContextResource {
        friend class Device;

    public:
        ~VertexArray() override;

        auto add_buffer(const graphic::VertexBuffer& vb, const graphic::VertexBufferLayout& layout) -> void override;
        auto add_instance_buffer(const graphic::VertexBuffer& vb) -> void override;
        auto bind() const -> void override;
        auto unbind() const -> void override;

    private:
        VertexArray(const std::shared_ptr<const Context>& context);

        uint32_t renderer_id  = 0; // GL vertex array handle
        uint32_t attrib_index = 0; // current attribute index
    };

} // namespace gkit::graphic::opengl
