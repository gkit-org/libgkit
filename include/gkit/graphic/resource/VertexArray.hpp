#pragma once

#include "gkit/graphic/VertexBufferLayout.hpp"
#include "gkit/graphic/resource/VertexBuffer.hpp"

/**
 * @brief Vertex array (frontend abstract interface)
 *
 * Holds the binding configuration of vertex buffers and their attribute layouts
 * (OpenGL VAO / Vulkan pipeline vertex input).
 */
namespace gkit::graphic {

    class VertexArray {
    public:
        VertexArray()                                      = default;
        VertexArray(const VertexArray&)                    = delete;
        auto operator=(const VertexArray&) -> VertexArray& = delete;

        VertexArray(VertexArray&&) noexcept                    = default;
        auto operator=(VertexArray&&) noexcept -> VertexArray& = default;

        virtual ~VertexArray() = default;

        /**
		 * @brief Bind a vertex buffer with its layout
		 */
        virtual auto add_buffer(const VertexBuffer& vb, const VertexBufferLayout& layout) -> void = 0;

        /**
		 * @brief Add an instance buffer (per-instance data)
		 */
        virtual auto add_instance_buffer(const VertexBuffer& vb) -> void = 0;

        /**
		 * @brief Bind to the current backend context
		 */
        virtual auto bind() const -> void = 0;

        /**
		 * @brief Unbind
		 */
        virtual auto unbind() const -> void = 0;
    };

} // namespace gkit::graphic
