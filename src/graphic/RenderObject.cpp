#include "gkit/graphic/render/RenderObject.hpp"

namespace gkit::graphic {

    RenderObject::RenderObject(const std::vector<float>& vertices,
                               const std::vector<uint32_t>& indices,
                               const VertexBufferLayout& layout,
                               const Material& material) :
        material(material), vertices(vertices), indices(indices), layout(layout) {}

    auto RenderObject::ensure_uploaded(RenderDevice& device) -> const VertexArray& {
        if (this->uploaded) {
            return *this->vao;
        }

        // Create and cache GPU resources (hidden from the user).
        this->vbo = device.create_vertex_buffer(this->vertices.data(), this->vertices.size() * sizeof(float), false);
        this->ibo = device.create_index_buffer(this->indices.data(), static_cast<uint32_t>(this->indices.size()));
        this->vao = device.create_vertex_array();
        this->vao->add_buffer(*this->vbo, this->layout);

        this->uploaded = true;
        return *this->vao;
    }

    auto RenderObject::index_buffer() -> const IndexBuffer& {
        return *this->ibo;
    }

} // namespace gkit::graphic
