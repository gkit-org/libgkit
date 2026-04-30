#include "gkit/graphic/opengl/VertexArray.hpp"
#include "gkit/graphic/opengl/IndexBuffer.hpp"
#include "gkit/graphic/opengl/config.hpp"
#include "gkit/graphic/Renderer.hpp"
#include "gkit/graphic/Shader.hpp"

#include <glad/gl.h>

auto gkit::graphic::Renderer::clear(opengl::ClearFlags flags) const -> void {
    auto mask = static_cast<GLbitfield>(flags);
    glClear(mask);
}

auto gkit::graphic::Renderer::draw(const gkit::graphic::opengl::VertexArray& va, const gkit::graphic::opengl::buffer::IndexBuffer& ib, const gkit::graphic::Shader& shader) const -> void {
    shader.bind();
    va.bind();
    ib.bind();
    glDrawElements(GL_TRIANGLES, ib.get_count(), GL_UNSIGNED_INT, nullptr);
}

auto gkit::graphic::Renderer::draw_instance(const gkit::graphic::opengl::VertexArray& va, const gkit::graphic::opengl::buffer::IndexBuffer& ib, const gkit::graphic::Shader& shader, uint32_t instanceCount) const -> void {
    shader.bind();
    va.bind();
    ib.bind();
    glDrawElementsInstanced(GL_TRIANGLES, ib.get_count(), GL_UNSIGNED_INT, nullptr, instanceCount);
}