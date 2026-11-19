#pragma once

#include "graphic/backend/opengl/context.hpp"
#include "graphic/backend/opengl/state_manager.hpp"
#include "graphic/render_device.hpp"

#include <memory>
#include <string>
#include <type_traits>

/**
 * @brief OpenGL backend render device
 *
 * Inherits frontend `graphic::RenderDevice`; implements the resource factory
 * and GL render commands. Owns a StateManager for incremental state application.
 */
namespace gkit::graphic::opengl {

    class Device final : public graphic::RenderDevice {
    public:
        explicit Device(platform::Window& window);

        [[nodiscard]] auto is_current() const noexcept -> bool override;
        auto bind_window_target() noexcept -> void override;
        auto clear_window(ClearFlags flags) -> void override;
        auto present() -> void override;
        auto validate_submission(const Material& material, const graphic::FrameBuffer* target) const -> void override;

        ~Device() override = default;

        auto create_vertex_buffer(const void* data, uint32_t size, bool dynamic)
            -> std::unique_ptr<graphic::VertexBuffer> override;
        auto create_index_buffer(const uint32_t* data, uint32_t count)
            -> std::unique_ptr<graphic::IndexBuffer> override;
        auto create_shader(const std::string& filepath) -> std::unique_ptr<graphic::Shader> override;
        auto create_sprite_shader() -> std::shared_ptr<graphic::Shader> override;
        auto upload_texture(const resource::Texture& image) -> std::shared_ptr<graphic::Texture> override;
        auto create_color_attachment(int width, int height) -> std::unique_ptr<graphic::Texture> override;
        auto create_vertex_array() -> std::unique_ptr<graphic::VertexArray> override;
        auto create_frame_buffer(int width, int height) -> std::unique_ptr<graphic::FrameBuffer> override;
        auto create_render_buffer(int width, int height) -> std::unique_ptr<graphic::RenderBuffer> override;

        auto clear(ClearFlags flags) -> void override;
        auto apply_state(const graphic::RenderState& state) -> void override;
        auto set_clip(std::optional<ViewportRect> clip) noexcept -> void override;
        auto set_viewport(const graphic::ViewportRect& viewport) -> void override;
        auto draw(const graphic::VertexArray& va, const graphic::IndexBuffer& ib, const graphic::Shader& shader)
            -> void override;
        auto draw_instance(const graphic::VertexArray& va,
                           const graphic::IndexBuffer& ib,
                           const graphic::Shader& shader,
                           uint32_t instance_count) -> void override;

    private:
        struct DestroyContext {
            auto operator()(SDL_GLContext handle) const noexcept -> void { SDL_GL_DestroyContext(handle); }
        };

        using ContextHandle = std::unique_ptr<std::remove_pointer_t<SDL_GLContext>, DestroyContext>;

        static auto create_context(SDL_Window* window) -> ContextHandle;

        SDL_Window* window;
        ContextHandle native;
        std::shared_ptr<const Context> context;
        StateManager state_manager; // Incremental GL state application
    };

} // namespace gkit::graphic::opengl
