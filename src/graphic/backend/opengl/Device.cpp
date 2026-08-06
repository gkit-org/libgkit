#include "graphic/backend/opengl/Device.hpp"

#include "graphic/backend/opengl/FrameBuffer.hpp"
#include "graphic/backend/opengl/IndexBuffer.hpp"
#include "graphic/backend/opengl/RenderBuffer.hpp"
#include "graphic/backend/opengl/Shader.hpp"
#include "graphic/backend/opengl/VertexArray.hpp"
#include "graphic/backend/opengl/VertexBuffer.hpp"
#include "graphic/backend/opengl/config.hpp"

#include <glad/gl.h>

namespace gkit::graphic::opengl {

    Device::Device() {
        this->state_manager.force_apply_all();
    }

    auto Device::create_vertex_buffer(const void* data, uint32_t size, bool dynamic)
        -> std::unique_ptr<graphic::VertexBuffer> {
        // Direct new (not make_unique) so Device's friendship grants access to the
        // private constructor; make_unique instantiates the new inside std::.
        return std::unique_ptr<graphic::VertexBuffer>(new VertexBuffer(data, size, dynamic));
    }

    auto Device::create_index_buffer(const uint32_t* data, uint32_t count) -> std::unique_ptr<graphic::IndexBuffer> {
        return std::unique_ptr<graphic::IndexBuffer>(new IndexBuffer(data, count));
    }

    auto Device::create_shader(const std::string& filepath) -> std::unique_ptr<graphic::Shader> {
        return std::unique_ptr<graphic::Shader>(new Shader(filepath));
    }

    auto Device::create_texture() -> std::unique_ptr<graphic::Texture> {
        // Empty until the resource module is ready (see RHI design doc §4.4/§4.5)
        return nullptr;
    }

    auto Device::create_vertex_array() -> std::unique_ptr<graphic::VertexArray> {
        return std::unique_ptr<graphic::VertexArray>(new VertexArray());
    }

    auto Device::create_frame_buffer(int width, int height) -> std::unique_ptr<graphic::FrameBuffer> {
        return std::unique_ptr<graphic::FrameBuffer>(new FrameBuffer(width, height));
    }

    auto Device::create_render_buffer(int width, int height) -> std::unique_ptr<graphic::RenderBuffer> {
        return std::unique_ptr<graphic::RenderBuffer>(new RenderBuffer(width, height));
    }

    auto Device::clear(ClearFlags flags) -> void {
        auto mask = to_gl_clear_mask(flags);
        glClear(mask);
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
