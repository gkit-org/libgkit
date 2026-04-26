#pragma once

#include "VertexBufferLayout.hpp"
#include "VertexBuffer.hpp"

#include <cstdint>

/**
 * @brief Vertex array wrapper for OpenGL vertex array objects (VAO)
 *
 * A VertexArray stores the configuration of vertex attributes and their
 * associated buffers, binding them together for efficient rendering.
 */
namespace gkit::graphic::opengl {
	
	class VertexArray
	{
	public:
		/**
		* @brief Construct a vertex array object
		*/
		VertexArray();

		/**
		* @brief Destructor - deletes the vertex array object
		*/
		~VertexArray();

		/**
		* @brief Add a vertex buffer with a layout
		* @param vb Vertex buffer to add
		* @param layout Layout defining the vertex attributes
		*/
		auto AddBuffer(const buffer::VertexBuffer& vb, const buffer::VertexBufferLayout& layout) -> void;

		/**
		* @brief Add an instance buffer for instanced rendering
		* @param vb Vertex buffer containing instance data
		*/
		auto AddInstanceBuffer(const buffer::VertexBuffer& vb) -> void;

		/**
		* @brief Bind this vertex array to the current OpenGL context
		*/
		auto Bind() const -> void;

		/**
		* @brief Unbind this vertex array from the current OpenGL context
		*/
		auto Unbind() const -> void;

	private:
		uint32_t m_RendererID;      	///< OpenGL vertex array ID
		uint32_t m_AttribIndex = 0;     ///< Current attribute index for adding new attributes
	};

} // namespace gkit::graphic::opengl