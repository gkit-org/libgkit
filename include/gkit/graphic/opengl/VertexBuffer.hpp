#pragma once

#include <cstdint>

/**
 * @brief Vertex buffer wrapper for OpenGL vertex buffer objects (VBO)
 *
 * A VertexBuffer stores vertex data on the GPU, containing attributes
 * such as position, color, texture coordinates, etc.
 */
namespace gkit::graphic::opengl::buffer {

	class VertexBuffer {
	private:
		uint32_t m_RendererID;                      ///< OpenGL buffer ID
		uint32_t m_Size;                            ///< Buffer size in bytes
	public:
		/**
		 * @brief Construct a vertex buffer
		 * @param data Pointer to vertex data
		 * @param size Size of the data in bytes
		 * @param Dynamic If true, the buffer will be updated frequently (GL_DYNAMIC_DRAW)
		 */
		VertexBuffer(const void* data, uint32_t size, bool Dynamic = false);

		/**
		 * @brief Destructor - deletes the vertex buffer
		 */
		~VertexBuffer();

		/**
		 * @brief Bind this vertex buffer to the current OpenGL context
		 */
		auto Bind() const -> void;

		/**
		 * @brief Unbind this vertex buffer from the current OpenGL context
		 */
		auto Unbind() const -> void;

		/**
		 * @brief Update all buffer data
		 * @param data Pointer to the new data
		 * @param size Size of the data in bytes (if size is same, uses SubData; if different, reallocates)
		 */
		auto UpdateData(const void* data, uint32_t size) -> void;

		/**
		 * @brief Update partial buffer data
		 * @param offset Offset in bytes from the start of the buffer
		 * @param data Pointer to the new data
		 * @param size Size of the data in bytes
		 */
		auto UpdateSubData(uint32_t offset, const void* data, uint32_t size) -> void;

	};

} // namespace gkit::graphic::opengl::buffer