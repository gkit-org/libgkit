#pragma once

#include <glad/gl.h>

#include <vector>
#include <cstdint>
#include <cassert>

/**
 * @brief Structure representing a single element in a vertex buffer layout
 *
 * Describes one attribute in the vertex data, including its type, count,
 * and whether it should be normalized.
 */
namespace gkit::graphic::opengl::buffer {

	struct VertexBufferElement {
		uint32_t type;         ///< OpenGL data type (GL_FLOAT, GL_UNSIGNED_INT, etc.)
		uint32_t count;        ///< Number of components in this element
		unsigned char normalized; ///< Whether the data should be normalized

		/**
		 * @brief Get the size in bytes of an OpenGL data type
		 * @param type OpenGL data type
		 * @return Size in bytes
		 */
		static constexpr auto get_size_of_type(uint32_t type) -> uint32_t {
			switch (type) {
				case GL_FLOAT: return 4;
				case GL_UNSIGNED_INT: return 4;
				case GL_UNSIGNED_BYTE: return 1;
				default: assert(false && "Unknown vertex buffer element type"); return 0;
			}
		}
	};

	/**
	 * @brief Vertex buffer layout class for defining vertex attribute configurations
	 *
	 * Defines how vertex data is organized in memory, specifying the type and
	 * count of each attribute and the stride between consecutive vertices.
	 */
	class VertexBufferLayout {
	private:
		std::vector<VertexBufferElement> m_Elements;  ///< List of vertex elements
		uint32_t m_Stride = 0;                       ///< Bytes between consecutive vertices
	public:
		VertexBufferLayout() = default;

		/**
		 * @brief Add an element to the layout (unsupported type - compile-time error)
		 * @tparam T Data type
		 * @param count Number of components
		 */
		template<typename T>
		void push(uint32_t count) {
			static_assert(sizeof(T) == 0, "Unsupported type for VertexBufferLayout::push");
		}

		/**
		 * @brief Get all elements in this layout (by const reference - zero copy)
		 * @return Const reference to vector of vertex buffer elements
		 */
		[[nodiscard]] inline auto get_elements() const -> const std::vector<VertexBufferElement>& { return m_Elements; }

		/**
		 * @brief Get the stride (bytes between vertices)
		 * @return Stride in bytes
		 */
		[[nodiscard]] inline auto get_stride() const -> uint32_t { return m_Stride; }
	};

	/**
	 * @brief Template specialization for pushing float elements
	 * @param count Number of float components (1-4)
	 */
	template<>
	inline void VertexBufferLayout::push<float>(uint32_t count) {
		assert(count > 0 && "VertexBufferLayout::push count must be greater than 0");
		m_Elements.push_back({ .type = GL_FLOAT, .count = count, .normalized = GL_FALSE });
		m_Stride += VertexBufferElement::get_size_of_type(GL_FLOAT) * count;
	}

	/**
	 * @brief Template specialization for pushing unsigned int elements
	 * @param count Number of unsigned int components
	 */
	template<>
	inline void VertexBufferLayout::push<uint32_t>(uint32_t count) {
		assert(count > 0 && "VertexBufferLayout::push count must be greater than 0");
		m_Elements.push_back({ .type = GL_UNSIGNED_INT, .count = count, .normalized = GL_FALSE });
		m_Stride += VertexBufferElement::get_size_of_type(GL_UNSIGNED_INT) * count;
	}

	/**
	 * @brief Template specialization for pushing unsigned byte elements
	 * @param count Number of unsigned byte components
	 */
	template<>
	inline void VertexBufferLayout::push<unsigned char>(uint32_t count) {
		assert(count > 0 && "VertexBufferLayout::push count must be greater than 0");
		m_Elements.push_back({ .type = GL_UNSIGNED_BYTE, .count = count, .normalized = GL_TRUE });
		m_Stride += VertexBufferElement::get_size_of_type(GL_UNSIGNED_BYTE) * count;
	}

} // namespace gkit::graphic::opengl::buffer