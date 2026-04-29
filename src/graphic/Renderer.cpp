#include "gkit/graphic/opengl/VertexArray.hpp"
#include "gkit/graphic/opengl/IndexBuffer.hpp"
#include "gkit/graphic/opengl/config.hpp"
#include "gkit/graphic/Renderer.hpp"
#include "gkit/graphic/Shader.hpp"

#include <glad/gl.h>

auto gkit::graphic::Renderer::Clear(opengl::ClearFlags flags) const -> void {
    auto mask = static_cast<GLbitfield>(flags);
    glClear(mask);
}

auto gkit::graphic::Renderer::Draw(const gkit::graphic::opengl::VertexArray& va, const gkit::graphic::opengl::buffer::IndexBuffer& ib, const gkit::graphic::Shader& shader) const -> void {
    shader.Bind();
    va.Bind();
    ib.Bind();
    glDrawElements(GL_TRIANGLES, ib.GetCount(), GL_UNSIGNED_INT, nullptr);
}

auto gkit::graphic::Renderer::DrawInstance(const gkit::graphic::opengl::VertexArray& va, const gkit::graphic::opengl::buffer::IndexBuffer& ib, const gkit::graphic::Shader& shader, uint32_t instanceCount) const -> void {
    shader.Bind();
    va.Bind();
    ib.Bind();
    glDrawElementsInstanced(GL_TRIANGLES, ib.GetCount(), GL_UNSIGNED_INT, nullptr, instanceCount);
}