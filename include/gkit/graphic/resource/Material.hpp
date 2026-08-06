#pragma once

#include "gkit/graphic/config.hpp"
#include "gkit/graphic/resource/Shader.hpp"
#include "gkit/graphic/resource/Texture.hpp"
#include "gkit/graphic/resource/UniformBuffer.hpp"

#include <array>
#include <cstdint>

namespace gkit::graphic {

    /**
	 * @brief A reusable material (shader + texture slots + uniforms)
	 *
	 * Shader and textures are held by pointer (referenced, not owned); their
	 * lifetime is managed by the resource system. A Material can be shared by
	 * multiple RenderObjects.
	 */
    struct Material {
        Shader* shader                                         = nullptr; // Shader (pointer reference, not owned)
        std::array<const Texture*, MAX_TEXTURE_SLOTS> textures = {}; // Texture slots (pointer reference)
        uint32_t texture_count                                 = 0; // Number of slots actually used
        UniformData uniforms; // Simple-path per-name uniforms
        UboBlock ubo; // Batch-path UBO reference
    };

} // namespace gkit::graphic
