#pragma once

#include "gkit/graphic/config.hpp"
#include "gkit/graphic/resource/shader.hpp"
#include "gkit/graphic/resource/texture.hpp"
#include "gkit/graphic/resource/uniform_buffer.hpp"

#include <array>
#include <cstdint>
#include <memory>
#include <string>
#include <utility>

namespace gkit::graphic {

    /**
     * @brief A reusable material (shader + texture slots + uniforms)
     *
     * Copies retain shader and texture ownership and have independent uniform values.
     */
    class Material {
    public:
        /**
         * @brief Set the shader used by this material. An empty pointer clears the shader.
         */
        auto set_shader(std::shared_ptr<Shader> shader) -> void { this->shader = std::move(shader); }

        /**
         * @brief The bound shader (nullptr if none)
         */
        [[nodiscard]] auto get_shader() const -> Shader* { return this->shader.get(); }

        /**
         * @brief Set a uniform value by name (simple path: name → value list)
         * @param name uniform name as declared in the shader
         * @param value type-erased uniform value (int/float/Vector/Matrix)
         */
        auto set_uniform(const std::string& name, const UniformValue& value) -> void {
            for (auto& [existing, v] : this->uniforms.values) {
                if (existing == name) {
                    v = value;
                    return;
                }
            }
            this->uniforms.values.emplace_back(name, value);
        }

        /**
         * @brief The uniform list (simple path), read-only
         */
        [[nodiscard]] auto get_uniforms() const -> const UniformData& { return this->uniforms; }

        /**
         * @brief Set the UBO block reference (batch path)
         */
        auto set_ubo(const UboBlock& ubo) -> void { this->ubo = ubo; }

        /**
         * @brief The UBO block reference (batch path), read-only
         */
        [[nodiscard]] auto get_ubo() const -> const UboBlock& { return this->ubo; }

        /**
         * @brief Append a texture to the next free slot (slot is managed internally,
         *        so callers can't create gaps or overflow the slot array)
         * @return False for an empty texture or when all slots are used.
         */
        auto set_texture(std::shared_ptr<const Texture> texture) -> bool {
            if (!texture) return false;
            for (auto& slot : textures) {
                if (slot) continue;
                slot = std::move(texture);
                return true;
            }
            return false;
        }

        /**
         * @brief Number of textures appended so far
         */
        [[nodiscard]] auto get_texture_count() const -> uint32_t {
            uint32_t count = 0;
            while (count < textures.size() && textures[count]) ++count;
            return count;
        }

        /**
         * @brief Texture bound at a slot (nullptr if the slot is unused or out of range)
         * @param slot texture slot index
         * @return the texture at that slot, or nullptr
         */
        [[nodiscard]] auto get_texture(uint32_t slot) const -> const Texture* {
            return slot < textures.size() ? textures[slot].get() : nullptr;
        }

        /**
         * @brief Whether the material references a valid shader (null shader is invalid)
         */
        [[nodiscard]] auto is_valid() const -> bool { return this->shader != nullptr && this->shader->is_valid(); }

    private:
        std::shared_ptr<Shader> shader;
        UniformData uniforms; // Simple-path per-name uniforms
        UboBlock ubo; // Batch-path UBO reference
        std::array<std::shared_ptr<const Texture>, MAX_TEXTURE_SLOTS> textures;
    };

} // namespace gkit::graphic
