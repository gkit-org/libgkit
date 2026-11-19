#pragma once

#include "gkit/graphic/config.hpp"
#include "gkit/graphic/render/render_state.hpp"
#include "gkit/graphic/render/viewport_rect.hpp"
#include "gkit/graphic/resource/frame_buffer.hpp"
#include "gkit/graphic/resource/index_buffer.hpp"
#include "gkit/graphic/resource/render_buffer.hpp"
#include "gkit/graphic/resource/shader.hpp"
#include "gkit/graphic/resource/texture.hpp"
#include "gkit/graphic/resource/vertex_array.hpp"
#include "gkit/graphic/resource/vertex_buffer.hpp"

#include <cstdint>
#include <memory>
#include <optional>
#include <string>

namespace gkit::platform {
    class Window;
}

namespace gkit::resource {
    class Texture;
}

namespace gkit::graphic {
    class Material;

    /**
     * @brief Abstract render device: resource factory + render command entry
     *
     * Concrete backends implement resource creation and rendering operations.
     */
    class RenderDevice {
    public:
        RenderDevice(const RenderDevice&)                    = delete;
        auto operator=(const RenderDevice&) -> RenderDevice& = delete;

        RenderDevice()                                           = default;
        RenderDevice(RenderDevice&&) noexcept                    = default;
        auto operator=(RenderDevice&&) noexcept -> RenderDevice& = default;

        virtual ~RenderDevice() = default;

        /** @brief Open a rendering session for a native window using the backend selected at build time. */
        static auto create_for_window(platform::Window& window) -> std::shared_ptr<RenderDevice>;

        /** @brief Whether this session can issue commands on the calling thread. */
        [[nodiscard]] virtual auto is_current() const noexcept -> bool = 0;

        /** @brief Restore the window as the default read and draw target. */
        virtual auto bind_window_target() noexcept -> void = 0;

        /** @brief Clear the window draw target while preserving the current read target. */
        virtual auto clear_window(ClearFlags flags) -> void = 0;

        /** @brief Present the completed frame on the session's window. */
        virtual auto present() -> void = 0;

        /** @brief Reject incompatible or expired shader, texture and target handles before queueing. */
        virtual auto validate_submission(const Material& material, const FrameBuffer* target) const -> void = 0;

        /** @brief Built-in textured quad shader with image sampler and a 3x3 corner-to-clip transform. */
        virtual auto create_sprite_shader() -> std::shared_ptr<Shader> = 0;

        /** @brief Upload a decoded RGBA image, respecting its row pitch. */
        virtual auto upload_texture(const resource::Texture& image) -> std::shared_ptr<Texture> = 0;

        // ---- Resource factory ----

        virtual auto create_vertex_buffer(const void* data, uint32_t size, bool dynamic)
            -> std::unique_ptr<VertexBuffer>                                                                   = 0;
        virtual auto create_index_buffer(const uint32_t* data, uint32_t count) -> std::unique_ptr<IndexBuffer> = 0;
        virtual auto create_shader(const std::string& filepath) -> std::unique_ptr<Shader>                     = 0;
        virtual auto create_color_attachment(int width, int height) -> std::unique_ptr<Texture>                = 0;
        virtual auto create_vertex_array() -> std::unique_ptr<VertexArray>                                     = 0;
        virtual auto create_frame_buffer(int width, int height) -> std::unique_ptr<FrameBuffer>                = 0;
        virtual auto create_render_buffer(int width, int height) -> std::unique_ptr<RenderBuffer>              = 0;
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
         * @brief Set the viewport for the next draw command
         */
        virtual auto set_viewport(const ViewportRect& viewport) -> void = 0;

        /** @brief Restrict rasterization to target pixels. Passing nullopt restores the full target. */
        virtual auto set_clip(std::optional<ViewportRect> clip) noexcept -> void = 0;

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

} // namespace gkit::graphic
