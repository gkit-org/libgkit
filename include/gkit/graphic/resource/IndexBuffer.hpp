#pragma once

#include "gkit/graphic/resource/Buffer.hpp"

#include <cstdint>
#include <utility>

namespace gkit::graphic {

    /**
	 * @brief Index data buffer (frontend abstract interface)
	 *
	 * Stores draw-order index data for efficient indexed geometry rendering.
	 */
    class IndexBuffer : public Buffer {
    public:
        IndexBuffer()                                      = default;
        IndexBuffer(const IndexBuffer&)                    = delete;
        auto operator=(const IndexBuffer&) -> IndexBuffer& = delete;

        IndexBuffer(IndexBuffer&& other) noexcept : Buffer(std::move(other)) {}
        auto operator=(IndexBuffer&& other) noexcept -> IndexBuffer& {
            if (this != &other) {
                Buffer::operator=(std::move(other));
            }
            return *this;
        }

        ~IndexBuffer() override = default;

        /**
		 * @brief Number of indices
		 */
        [[nodiscard]] virtual auto get_count() const -> uint32_t = 0;
    };

} // namespace gkit::graphic
