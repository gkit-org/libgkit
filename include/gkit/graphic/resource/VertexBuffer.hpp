#pragma once

#include "gkit/graphic/resource/Buffer.hpp"

#include <utility>

namespace gkit::graphic {

    /**
	 * @brief Vertex data buffer (frontend abstract interface)
	 *
	 * Buffer that stores vertex data (position/color/uv, etc.) on the GPU.
	 */
    class VertexBuffer : public Buffer {
    public:
        VertexBuffer()                                       = default;
        VertexBuffer(const VertexBuffer&)                    = delete;
        auto operator=(const VertexBuffer&) -> VertexBuffer& = delete;

        VertexBuffer(VertexBuffer&& other) noexcept : Buffer(std::move(other)) {}
        auto operator=(VertexBuffer&& other) noexcept -> VertexBuffer& {
            if (this != &other) {
                Buffer::operator=(std::move(other));
            }
            return *this;
        }

        ~VertexBuffer() override = default;

        /**
		 * @brief Whether this is a dynamic buffer (updated frequently; backend maps to GL_DYNAMIC_DRAW etc.)
		 */
        [[nodiscard]] virtual auto is_dynamic() const -> bool = 0;
    };

} // namespace gkit::graphic
