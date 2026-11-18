#include "gkit/graphic/render/render_object.hpp"

#include "graphic/render_device.hpp"

namespace gkit::graphic {

    RenderObject::RenderObject(const std::vector<float>& vertices,
                               const std::vector<uint32_t>& indices,
                               const VertexBufferLayout& layout,
                               const Material& material) :
        material(material), vertices(vertices), indices(indices), layout(layout) {}

    auto RenderObject::ensure_uploaded(RenderDevice& device, const std::shared_ptr<const void>& session) -> void {
        if (uploaded_session.lock() == session) return;
        auto vertices_buffer = device.create_vertex_buffer(vertices.data(), vertices.size() * sizeof(float), false);
        auto vertex_array    = device.create_vertex_array();
        vertex_array->add_buffer(*vertices_buffer, layout);
        auto indices_buffer = device.create_index_buffer(indices.data(), static_cast<uint32_t>(indices.size()));
        vao                 = std::move(vertex_array);
        vbo                 = std::move(vertices_buffer);
        ibo                 = std::move(indices_buffer);
        uploaded_session    = session;
    }

} // namespace gkit::graphic
