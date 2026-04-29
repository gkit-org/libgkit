#pragma once

#include <cstdint>

namespace gkit::graphic::opengl::buffer{
	/**
	* @brief Index buffer wrapper for OpenGL element array buffers
	*
	* An IndexBuffer stores indices that determine the order in which vertices
	* are drawn, enabling efficient rendering of indexed geometry.
	*/
	class IndexBuffer {
	public:
		IndexBuffer(const IndexBuffer&) = delete;
		IndexBuffer& operator=(const IndexBuffer&) = delete;

		/** @brief Move constructor - transfers ownership of GL buffer
		 *  @param other Source object to move from (will be invalidated)
		 */
		IndexBuffer(IndexBuffer&& other) noexcept;

		/** @brief Move assignment - transfers ownership of GL buffer
		 *  @param other Source object to move from (will be invalidated)
		 *  @note Releases any existing GL buffer before taking ownership
		 */
		auto operator=(IndexBuffer&& other) noexcept -> IndexBuffer&;

	private:
		uint32_t m_RendererID;	///< OpenGL buffer ID
		uint32_t m_Count;     	///< Number of indices in the buffer
	public:
		/**
		 * @brief Construct an index buffer
		 * @param data Pointer to index data
		 * @param count Number of indices
		 */
		IndexBuffer(const uint32_t* data, uint32_t count);

		/**
		 * @brief Destructor - deletes the index buffer
		 */
		~IndexBuffer();

		/**
		 * @brief Bind this index buffer to the current OpenGL context
		 */
		auto Bind() const -> void;

		/**
		 * @brief Unbind this index buffer from the current OpenGL context
		 */
		auto Unbind() const -> void;

		/**
		 * @brief Get the number of indices in this buffer
		 * @return Number of indices
		 */
		[[nodiscard]] inline uint32_t GetCount() const { return m_Count; }
	};

} // namespace gkit::graphic::opengl::buffer