#pragma once

#include "gkit/graphic/VertexBufferLayout.hpp"
#include "gkit/graphic/resource/VertexArray.hpp"
#include "gkit/graphic/resource/VertexBuffer.hpp"

#include <cstdint>

/**
 * @brief OpenGL backend vertex array (VAO)
 *
 * Inherits frontend `graphic::VertexArray`; maintains GL vertex attribute
 * configuration.
 */
namespace gkit::graphic::opengl {

    class VertexArray final : public graphic::VertexArray {
        friend class Device;

    public:
        VertexArray(VertexArray&& other) noexcept;
        auto operator=(VertexArray&& other) noexcept -> VertexArray&;

        ~VertexArray() override;

        auto add_buffer(const graphic::VertexBuffer& vb, const graphic::VertexBufferLayout& layout) -> void override;
        auto add_instance_buffer(const graphic::VertexBuffer& vb) -> void override;
        auto bind() const -> void override;
        auto unbind() const -> void override;

    private:
        VertexArray();

        uint32_t renderer_id  = 0; // GL vertex array handle
        uint32_t attrib_index = 0; // current attribute index
    };

} // namespace gkit::graphic::opengl
