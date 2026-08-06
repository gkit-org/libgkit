#pragma once

#include <cstdint>

/**
 * @brief Base class for GPU data buffers (frontend abstract interface)
 *
 * Common data operations shared by all GPU data buffers (vertex, index, etc.).
 * Concrete backends (OpenGL/Vulkan) inherit and implement bind/unbind/update.
 * The resource handle (renderer_id) only lives inside the backend implementation.
 */
namespace gkit::graphic {

    class Buffer {
    public:
        Buffer()                                 = default;
        Buffer(const Buffer&)                    = delete;
        auto operator=(const Buffer&) -> Buffer& = delete;

        Buffer(Buffer&& other) noexcept : size(other.size) { other.size = 0; }
        auto operator=(Buffer&& other) noexcept -> Buffer& {
            if (this != &other) {
                this->size = other.size;
                other.size = 0;
            }
            return *this;
        }

        virtual ~Buffer() = default;

        /**
		 * @brief Bind to the current backend context
		 */
        virtual auto bind() const -> void = 0;

        /**
		 * @brief Unbind
		 */
        virtual auto unbind() const -> void = 0;

        /**
		 * @brief Update the whole buffer; uses SubData if size is unchanged, otherwise reallocates
		 */
        virtual auto update_data(const void* data, uint32_t size) -> void = 0;

        /**
		 * @brief Update a byte range starting at offset
		 */
        virtual auto update_sub_data(uint32_t offset, const void* data, uint32_t size) -> void = 0;

        /**
		 * @brief Buffer size in bytes
		 */
        [[nodiscard]] auto get_size() const -> uint32_t { return size; }

    protected:
        uint32_t size = 0; // size in bytes
    };

} // namespace gkit::graphic
