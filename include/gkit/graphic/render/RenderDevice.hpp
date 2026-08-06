#pragma once

#include "gkit/graphic/config.hpp"
#include "gkit/graphic/render/RenderCommand.hpp"
#include "gkit/graphic/render/RenderState.hpp"
#include "gkit/graphic/resource/FrameBuffer.hpp"
#include "gkit/graphic/resource/IndexBuffer.hpp"
#include "gkit/graphic/resource/RenderBuffer.hpp"
#include "gkit/graphic/resource/Shader.hpp"
#include "gkit/graphic/resource/Texture.hpp"
#include "gkit/graphic/resource/VertexArray.hpp"
#include "gkit/graphic/resource/VertexBuffer.hpp"

#include <cstdint>
#include <memory>
#include <string>

namespace gkit::graphic {

    /**
	 * @brief Backend identifier
	 */
    enum class Backend : std::uint8_t {
        OpenGL,
        // Vulkan  // future
    };

    /**
	 * @brief Abstract render device: resource factory + render command entry
	 *
	 * Concrete backends (OpenGL/Vulkan) inherit and implement resource creation
	 * and render operations. Renderer owns a RenderDevice selected via create_device.
	 */
    class RenderDevice {
    public:
        RenderDevice(const RenderDevice&)                    = delete;
        auto operator=(const RenderDevice&) -> RenderDevice& = delete;

        RenderDevice()                                           = default;
        RenderDevice(RenderDevice&&) noexcept                    = default;
        auto operator=(RenderDevice&&) noexcept -> RenderDevice& = default;

        virtual ~RenderDevice() = default;

        // ---- Resource factory ----

        virtual auto create_vertex_buffer(const void* data, uint32_t size, bool dynamic)
            -> std::unique_ptr<VertexBuffer>                                                                   = 0;
        virtual auto create_index_buffer(const uint32_t* data, uint32_t count) -> std::unique_ptr<IndexBuffer> = 0;
        virtual auto create_shader(const std::string& filepath) -> std::unique_ptr<Shader>                     = 0;
        /**
		 * @brief Create a texture; empty until the resource module is ready (see RHI design doc §4.4/§4.5)
		 */
        virtual auto create_texture() -> std::unique_ptr<Texture>                                 = 0;
        virtual auto create_vertex_array() -> std::unique_ptr<VertexArray>                        = 0;
        virtual auto create_frame_buffer(int width, int height) -> std::unique_ptr<FrameBuffer>   = 0;
        virtual auto create_render_buffer(int width, int height) -> std::unique_ptr<RenderBuffer> = 0;
        // future: create_storage_buffer() / create_uniform_buffer()

        // ---- Render command entry ----

        /**
		 * @brief Clear the current render target
		 */
        virtual auto clear(ClearFlags flags) -> void = 0;

        /**
		 * @brief Apply a render state snapshot incrementally
		 *
		 * Backends compare against the previously applied state and only change
		 * what differs (see RHI design doc §6 / render queue design §3.4).
		 */
        virtual auto apply_state(const RenderState& state) -> void = 0;

        /**
		 * @brief Set the viewport (GL viewport is global state, set per command)
		 */
        virtual auto set_viewport(const Viewport& viewport) -> void = 0;

        /**
		 * @brief Draw indexed geometry
		 */
        virtual auto draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader) -> void = 0;

        /**
		 * @brief Draw instanced indexed geometry
		 */
        virtual auto draw_instance(const VertexArray& va,
                                   const IndexBuffer& ib,
                                   const Shader& shader,
                                   uint32_t instance_count) -> void = 0;
    };

    /**
	 * @brief Backend factory - the single switch point across the project
	 */
    auto create_device(Backend backend) -> std::unique_ptr<RenderDevice>;

} // namespace gkit::graphic
