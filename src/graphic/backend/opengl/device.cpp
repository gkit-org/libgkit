#include "graphic/backend/opengl/device.hpp"

#include "gkit/graphic/resource/material.hpp"
#include "gkit/resource/texture.hpp"
#include "graphic/backend/opengl/config.hpp"
#include "graphic/backend/opengl/frame_buffer.hpp"
#include "graphic/backend/opengl/index_buffer.hpp"
#include "graphic/backend/opengl/render_buffer.hpp"
#include "graphic/backend/opengl/shader.hpp"
#include "graphic/backend/opengl/texture.hpp"
#include "graphic/backend/opengl/vertex_array.hpp"
#include "graphic/backend/opengl/vertex_buffer.hpp"
#include "platform/window_access.hpp"

#include <utility>

#include <glad/gl.h>

namespace gkit::graphic::opengl {

    namespace {
        // clang-format off
        const ShaderProgramSource SPRITE_SHADER{
            .vertex_shader = R"glsl(#version 410 core
layout(location = 0) in vec2 corner;
uniform mat3 transform;
out vec2 uv;
void main() {
    uv = corner;
    gl_Position = vec4((transform * vec3(corner, 1.0)).xy, 0.0, 1.0);
}
)glsl",
            .fragment_shader = R"glsl(#version 410 core
in vec2 uv;
uniform sampler2D image;
out vec4 color;
void main() {
    color = texture(image, uv);
}
)glsl"};
        // clang-format on
    } // namespace

    auto Device::create_context(SDL_Window* window) -> ContextHandle {
        platform::require_main_thread();
        if (!(SDL_GetWindowFlags(window) & SDL_WINDOW_OPENGL))
            throw std::invalid_argument("the OpenGL backend requires an OpenGL window");
        if (SDL_GL_GetCurrentContext()) throw std::logic_error("an OpenGL context is already current");
        ContextHandle native(SDL_GL_CreateContext(window));
        if (!native) throw std::runtime_error(std::string("SDL_GL_CreateContext: ") + SDL_GetError());
        if (gladLoadGL(SDL_GL_GetProcAddress) < GLAD_MAKE_VERSION(4, 1))
            throw std::runtime_error("the OpenGL backend requires OpenGL 4.1");
        return native;
    }


    Device::Device(platform::Window& target) :
        window(platform::detail::WindowAccess::native(target)), native(create_context(window)),
        context(std::make_shared<Context>(Context{native.get()})) {
        state_manager.force_apply_all();
    }


    auto Device::is_current() const noexcept -> bool {
        return context->is_current();
    }


    auto Device::bind_window_target() noexcept -> void {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }


    auto Device::clear_window(ClearFlags flags) -> void {
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        clear(flags);
    }


    auto Device::present() -> void {
        if (!SDL_GL_SwapWindow(window)) throw std::runtime_error(std::string("SDL_GL_SwapWindow: ") + SDL_GetError());
    }


    auto Device::validate_submission(const Material& material, const graphic::FrameBuffer* target) const -> void {
        if (target) require_resource<FrameBuffer>(*target);
        const auto* shader = material.get_shader();
        if (!shader) throw std::invalid_argument("render object requires a shader");
        require_resource<Shader>(*shader);
        if (!shader->is_valid()) throw std::invalid_argument("render object requires a valid shader");
        for (std::uint32_t slot = 0; slot < material.get_texture_count(); ++slot)
            require_resource<Texture>(*material.get_texture(slot));
    }


    auto Device::create_vertex_buffer(const void* data, uint32_t size, bool dynamic)
        -> std::unique_ptr<graphic::VertexBuffer> {
        // Direct new (not make_unique) so Device's friendship grants access to the
        // private constructor; make_unique instantiates the new inside std::.
        return std::unique_ptr<graphic::VertexBuffer>(new VertexBuffer(context, data, size, dynamic));
    }


    auto Device::create_index_buffer(const uint32_t* data, uint32_t count) -> std::unique_ptr<graphic::IndexBuffer> {
        return std::unique_ptr<graphic::IndexBuffer>(new IndexBuffer(context, data, count));
    }


    auto Device::create_shader(const std::string& filepath) -> std::unique_ptr<graphic::Shader> {
        return std::unique_ptr<graphic::Shader>(new Shader(context, filepath));
    }


    auto Device::create_sprite_shader() -> std::shared_ptr<graphic::Shader> {
        return std::shared_ptr<graphic::Shader>(new Shader(context, SPRITE_SHADER));
    }


    auto Device::upload_texture(const resource::Texture& image) -> std::shared_ptr<graphic::Texture> {
        return std::shared_ptr<graphic::Texture>(
            new Texture(context, image.width(), image.height(), image.pixels().data(), image.pitch()));
    }


    auto Device::create_color_attachment(int width, int height) -> std::unique_ptr<graphic::Texture> {
        return std::unique_ptr<graphic::Texture>(new Texture(context, width, height));
    }


    auto Device::create_vertex_array() -> std::unique_ptr<graphic::VertexArray> {
        return std::unique_ptr<graphic::VertexArray>(new VertexArray(context));
    }


    auto Device::create_frame_buffer(int width, int height) -> std::unique_ptr<graphic::FrameBuffer> {
        return std::unique_ptr<graphic::FrameBuffer>(new FrameBuffer(context, width, height));
    }


    auto Device::create_render_buffer(int width, int height) -> std::unique_ptr<graphic::RenderBuffer> {
        return std::unique_ptr<graphic::RenderBuffer>(new RenderBuffer(context, width, height));
    }


    auto Device::clear(ClearFlags flags) -> void {
        const auto mask      = to_gl_clear_mask(flags);
        GLboolean depth_mask = GL_TRUE;
        GLint stencil_mask = 0, back_stencil_mask = 0;
        if (mask & GL_DEPTH_BUFFER_BIT) {
            glGetBooleanv(GL_DEPTH_WRITEMASK, &depth_mask);
            glDepthMask(GL_TRUE);
        }
        if (mask & GL_STENCIL_BUFFER_BIT) {
            glGetIntegerv(GL_STENCIL_WRITEMASK, &stencil_mask);
            glGetIntegerv(GL_STENCIL_BACK_WRITEMASK, &back_stencil_mask);
            glStencilMask(~GLuint{0});
        }
        glClear(mask);
        if (mask & GL_DEPTH_BUFFER_BIT) glDepthMask(depth_mask);
        if (mask & GL_STENCIL_BUFFER_BIT) {
            glStencilMaskSeparate(GL_FRONT, static_cast<GLuint>(stencil_mask));
            glStencilMaskSeparate(GL_BACK, static_cast<GLuint>(back_stencil_mask));
        }
    }


    auto Device::apply_state(const graphic::RenderState& state) -> void {
        this->state_manager.apply(state);
    }


    auto Device::set_clip(std::optional<ViewportRect> clip) noexcept -> void {
        if (clip) {
            glEnable(GL_SCISSOR_TEST);
            glScissor(clip->x, clip->y, clip->width, clip->height);
        } else {
            glDisable(GL_SCISSOR_TEST);
        }
    }


    auto Device::set_viewport(const graphic::ViewportRect& viewport) -> void {
        glViewport(viewport.x, viewport.y, viewport.width, viewport.height);
    }


    auto Device::draw(const graphic::VertexArray& va, const graphic::IndexBuffer& ib, const graphic::Shader& shader)
        -> void {
        shader.bind();
        va.bind();
        ib.bind();
        glDrawElements(GL_TRIANGLES, ib.get_count(), GL_UNSIGNED_INT, nullptr);
    }


    auto Device::draw_instance(const graphic::VertexArray& va,
                               const graphic::IndexBuffer& ib,
                               const graphic::Shader& shader,
                               uint32_t instance_count) -> void {
        shader.bind();
        va.bind();
        ib.bind();
        glDrawElementsInstanced(GL_TRIANGLES, ib.get_count(), GL_UNSIGNED_INT, nullptr, instance_count);
    }

} // namespace gkit::graphic::opengl

namespace gkit::graphic {
    auto RenderDevice::create_for_window(platform::Window& window) -> std::shared_ptr<RenderDevice> {
        return std::make_shared<opengl::Device>(window);
    }
} // namespace gkit::graphic
