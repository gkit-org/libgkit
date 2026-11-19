#pragma once

#include "gkit/graphic/config.hpp"
#include "gkit/graphic/render/render_state.hpp"
#include "gkit/graphic/resource/index_buffer.hpp"
#include "gkit/graphic/resource/material.hpp"
#include "gkit/graphic/resource/vertex_array.hpp"
#include "gkit/graphic/resource/vertex_buffer.hpp"
#include "gkit/graphic/vertex_buffer_layout.hpp"

#include <cstdint>
#include <memory>
#include <vector>

namespace gkit::graphic {
    class RenderDevice;
    class RenderQueue;

    /**
     * @brief A draw unit defined by CPU data (vertices/indices + material + state)
     *
     * Users provide vertex/index arrays, a vertex layout, a material, and state.
     * The VAO/VBO/IBO creation and binding are hidden: GPU resources are lazily
     * created and cached on first draw in each graphics session.
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
        RenderObject(const RenderObject&)                    = delete;
        auto operator=(const RenderObject&) -> RenderObject& = delete;

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

        /** @brief Whether the cached geometry belongs to a live graphics session. Coordinator thread only. */
        [[nodiscard]] auto is_uploaded() const -> bool { return !uploaded_session.expired(); }

    private:
        friend class RenderQueue;

        auto ensure_uploaded(RenderDevice& device, const std::shared_ptr<const void>& session) -> void;

        // CPU geometry data (provided by the user)
        std::vector<float> vertices;
        std::vector<uint32_t> indices;
        VertexBufferLayout layout;

        // Lazily created GPU resources (hidden from the user)
        std::unique_ptr<VertexBuffer> vbo;
        std::unique_ptr<VertexArray> vao;
        std::unique_ptr<IndexBuffer> ibo;
        std::weak_ptr<const void> uploaded_session;
    };

} // namespace gkit::graphic
