#include "gkit/graphic/opengl/VertexArray.hpp"
#include "gkit/graphic/opengl/VertexBuffer.hpp"
#include "gkit/graphic/opengl/VertexBufferLayout.hpp"
#include "gkit/math/vector4.hpp"
#include "gkit/math/matrix4.hpp"

gkit::graphic::opengl::VertexArray::VertexArray() {
	glGenVertexArrays(1, &m_RendererID);
}

gkit::graphic::opengl::VertexArray::~VertexArray() {
	if(m_RendererID != 0) {
		glDeleteVertexArrays(1, &m_RendererID);
		m_RendererID = 0;
	}
}

auto gkit::graphic::opengl::VertexArray::AddBuffer(const buffer::VertexBuffer& vb, const buffer::VertexBufferLayout& layout) -> void {
	Bind();
	vb.Bind();
	const auto& elements = layout.GetElements();
	size_t offset = 0;
	for (int i = 0; i < elements.size(); i++) {
		const auto& element = elements[i];
		glEnableVertexAttribArray(i);
		//Specify the reading rules for the incoming data, the meaning of the arrays (coordinates, textures, etc.)
		glVertexAttribPointer(m_AttribIndex, element.count, element.type,
			element.normalized, layout.GetStride(), (const void*)offset);
		offset += element.count * buffer::VertexBufferElement::GetSizeOfType(element.type);
		m_AttribIndex++;
	}

}

auto gkit::graphic::opengl::VertexArray::AddInstanceBuffer(const buffer::VertexBuffer& vb) -> void {
    Bind();
	vb.Bind();

    size_t vec4Size = sizeof(gkit::math::Vector4);

    for (uint32_t i = 0; i < 4; i++) {
        glEnableVertexAttribArray(m_AttribIndex);
        glVertexAttribPointer(
			m_AttribIndex,
            4,
            GL_FLOAT,
            GL_FALSE,
            sizeof(gkit::math::Matrix4),
            (void*)(i * vec4Size)
        );
        glVertexAttribDivisor(m_AttribIndex, 1);
		m_AttribIndex++;
    }
}

auto gkit::graphic::opengl::VertexArray::Bind() const -> void {
	glBindVertexArray(m_RendererID);
}

auto gkit::graphic::opengl::VertexArray::Unbind() const -> void {
	glBindVertexArray(0);
}
