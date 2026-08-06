#pragma once

#include "gkit/graphic/VertexBufferLayout.hpp"
#include "gkit/graphic/config.hpp"
#include "gkit/graphic/render/RenderDevice.hpp"
#include "gkit/graphic/render/RenderState.hpp"
#include "gkit/graphic/resource/IndexBuffer.hpp"
#include "gkit/graphic/resource/Material.hpp"
#include "gkit/graphic/resource/VertexArray.hpp"
#include "gkit/graphic/resource/VertexBuffer.hpp"

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
        uint32_t instance_count = 1; // 1 = non-instanced
        bool transparent        = false; // Sorting class
        float depth_key         = 0.0f; // Depth sort key
        bool clear              = false; // Whether to clear the target before drawing
        ClearFlags clear_flags  = ClearFlags::All; // What to clear when clear is set

        /**
		 * @brief Lazily upload vertices/indices to GPU and return the vertex array
		 * @param device backend device used to create GPU buffers
		 * @return reference to the cached vertex array
		 */
        auto ensure_uploaded(RenderDevice& device) -> const VertexArray&;

        /// @brief Access the cached index buffer (valid after ensure_uploaded)
        [[nodiscard]] auto index_buffer() -> const IndexBuffer&;

        /// @brief Whether GPU resources have been created
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
