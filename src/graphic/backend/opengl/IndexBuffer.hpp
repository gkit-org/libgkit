#pragma once

#include "gkit/graphic/resource/IndexBuffer.hpp"

#include <cstdint>

/**
 * @brief OpenGL backend index buffer (EBO)
 *
 * Inherits frontend `graphic::IndexBuffer`; implements the concrete GL buffer
 * operations.
 */
namespace gkit::graphic::opengl {

    class IndexBuffer final : public graphic::IndexBuffer {
        friend class Device;

    public:
        IndexBuffer(IndexBuffer&& other) noexcept;
        auto operator=(IndexBuffer&& other) noexcept -> IndexBuffer&;

        ~IndexBuffer() override;

        auto bind() const -> void override;
        auto unbind() const -> void override;
        auto update_data(const void* data, uint32_t size) -> void override;
        auto update_sub_data(uint32_t offset, const void* data, uint32_t size) -> void override;
        [[nodiscard]] auto get_count() const -> uint32_t override { return this->count; }

    private:
        /**
		 * @brief Construct an index buffer (only Device may create one)
		 * @param data pointer to index data
		 * @param count number of indices
		 */
        explicit IndexBuffer(const uint32_t* data, uint32_t count);

        uint32_t renderer_id = 0; // GL buffer handle
        uint32_t count       = 0; // number of indices
    };

} // namespace gkit::graphic::opengl
