#pragma once

#include "gkit/graphic/resource/VertexBuffer.hpp"

#include <cstdint>

/**
 * @brief OpenGL backend vertex buffer (VBO)
 *
 * Inherits frontend `graphic::VertexBuffer`; implements the concrete GL buffer
 * operations.
 */
namespace gkit::graphic::opengl {

    class VertexBuffer final : public graphic::VertexBuffer {
        friend class Device;

    public:
        VertexBuffer(VertexBuffer&& other) noexcept;
        auto operator=(VertexBuffer&& other) noexcept -> VertexBuffer&;

        ~VertexBuffer() override;

        auto bind() const -> void override;
        auto unbind() const -> void override;
        auto update_data(const void* data, uint32_t size) -> void override;
        auto update_sub_data(uint32_t offset, const void* data, uint32_t size) -> void override;
        [[nodiscard]] auto is_dynamic() const -> bool override { return this->dynamic; }

    private:
        /**
		 * @brief Construct a vertex buffer (only Device may create one)
		 * @param data pointer to vertex data
		 * @param size size of the data in bytes
		 * @param dynamic true if updated frequently (GL_DYNAMIC_DRAW)
		 */
        explicit VertexBuffer(const void* data, uint32_t size, bool dynamic = false);

        uint32_t renderer_id = 0; // GL buffer handle
        bool dynamic         = false;
    };

} // namespace gkit::graphic::opengl
