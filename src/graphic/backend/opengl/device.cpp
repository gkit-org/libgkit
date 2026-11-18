#include "graphic/backend/opengl/device.hpp"

#include "graphic/backend/opengl/config.hpp"
#include "graphic/backend/opengl/frame_buffer.hpp"
#include "graphic/backend/opengl/index_buffer.hpp"
#include "graphic/backend/opengl/render_buffer.hpp"
#include "graphic/backend/opengl/shader.hpp"
#include "graphic/backend/opengl/texture.hpp"
#include "graphic/backend/opengl/vertex_array.hpp"
#include "graphic/backend/opengl/vertex_buffer.hpp"

#include <utility>

#include <glad/gl.h>

namespace gkit::graphic::opengl {

    Device::Device(std::shared_ptr<const Context> context) : context(std::move(context)) {
        this->state_manager.force_apply_all();
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

    auto Device::set_viewport(const graphic::Viewport& viewport) -> void {
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
