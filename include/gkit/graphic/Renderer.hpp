#pragma once

#include "opengl/IndexBuffer.hpp"
#include "opengl/VertexArray.hpp"
#include "Shader.hpp"
#include "opengl/config.hpp"
#include "gkit/core/scene/singleton.hpp"

#include <cstdint>

/**
 * @brief Renderer class providing public rendering interface
 *
 * The Renderer provides a unified interface for rendering operations.
 * Uses singleton pattern for global access.
 */
namespace gkit::graphic {

	class Renderer : public core::scene::Singleton<Renderer> {
	public:
		Renderer() = default;

		/** @brief Get singleton instance */
		static auto Get() -> Renderer& { return instance(); }

		/**
		 * @brief Clear the current framebuffer
		 *
		 * @param flags Bitmask specifying which buffers to clear (e.g., ClearFlags::Color | ClearFlags::Depth)
		 *              Defaults to ClearFlags::All (clears all buffers)
		 */
		auto Clear(opengl::ClearFlags flags = opengl::ClearFlags::All) const -> void;

		/**
		 * @brief Draw indexed geometry
		 * @param va Vertex array containing vertex data
		 * @param ib Index buffer containing indices
		 * @param shader Shader program to use for rendering
		 */
		auto Draw(const gkit::graphic::opengl::VertexArray& va, const gkit::graphic::opengl::buffer::IndexBuffer& ib, const gkit::graphic::Shader& shader) const -> void;

		/**
		 * @brief Draw multiple instances of indexed geometry
		 * @param va Vertex array containing vertex data
		 * @param ib Index buffer containing indices
		 * @param shader Shader program to use for rendering
		 * @param instanceCount Number of instances to draw
		 */
		auto DrawInstance(const gkit::graphic::opengl::VertexArray& va, const gkit::graphic::opengl::buffer::IndexBuffer& ib, const gkit::graphic::Shader& shader, uint32_t instanceCount) const -> void;
	};

} // namespace gkit::graphic