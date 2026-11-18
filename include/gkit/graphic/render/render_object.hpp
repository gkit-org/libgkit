#pragma once

#include "gkit/graphic/vertex_buffer_layout.hpp"
#include "gkit/graphic/config.hpp"
#include "gkit/graphic/render/render_device.hpp"
#include "gkit/graphic/render/render_state.hpp"
#include "gkit/graphic/resource/index_buffer.hpp"
#include "gkit/graphic/resource/material.hpp"
#include "gkit/graphic/resource/vertex_array.hpp"
#include "gkit/graphic/resource/vertex_buffer.hpp"

#include <cstdint>
#include <memory>
#include <vector>

namespace gkit::graphic {

    /**
     * @brief A draw unit defined by CPU data (vertices/indices + material + state)
     *
     * Users provide vertex/index arrays, a vertex layout, a material, and state.
     * The VAO/VBO/IBO creation and binding are hidden: GPU resources are lazily
     * created and cached on first draw. Only a RenderCommand built from this
     * object is enqueued.
     */
    class RenderObject {
    public:
        /**
         * @brief Construct from CPU geometry data, layout, and material
         * @param vertices interleaved vertex data
         * @param indices index data
         * @param layout vertex attribute layout (position/color/uv...)
         * @param material material (shader + textures + uniforms)
         * @note vertices/indices are copied into the object (owned CPU data).
         */
        RenderObject(const std::vector<float>& vertices,
                     const std::vector<uint32_t>& indices,
                     const VertexBufferLayout& layout,
                     const Material& material);

        // ---- Material (reusable, replaceable) ----
        Material material;

        // ---- Render state / target ----
        RenderState state;
        /** @brief 1 = non-instanced. */
        uint32_t instance_count = 1;
        /** @brief Sorting class. */
        bool transparent = false;
        /** @brief Depth sort key. */
        float depth_key = 0.0f;
        /** @brief Whether to clear the target before drawing. */
        bool clear = false;
        /** @brief What to clear when clear is set. */
        ClearFlags clear_flags = ClearFlags::All;

        /**
         * @brief Lazily upload vertices/indices to GPU and return the vertex array
         * @param device backend device used to create GPU buffers
         * @return reference to the cached vertex array
         */
        auto ensure_uploaded(RenderDevice& device) -> const VertexArray&;

        /**
         * @brief Access the cached index buffer (valid after ensure_uploaded)
         */
        [[nodiscard]] auto index_buffer() -> const IndexBuffer&;

        /**
         * @brief Whether GPU resources have been created
         */
        [[nodiscard]] auto is_uploaded() const -> bool { return this->uploaded; }

    private:
        // CPU geometry data (provided by the user)
        std::vector<float> vertices;
        std::vector<uint32_t> indices;
        VertexBufferLayout layout;

        // Lazily created GPU resources (hidden from the user)
        std::unique_ptr<VertexBuffer> vbo;
        std::unique_ptr<VertexArray> vao;
        std::unique_ptr<IndexBuffer> ibo;
        bool uploaded = false;
    };

} // namespace gkit::graphic
