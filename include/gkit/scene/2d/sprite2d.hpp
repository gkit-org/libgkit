#pragma once

#include "gkit/resource/texture.hpp"
#include "gkit/scene/2d/object2d.hpp"

namespace gkit::scene {
    /** @brief Image drawn automatically by Application at its transformed top-left corner.
     * Size is measured in logical pixels. An unconfigured sprite draws nothing. */
    class Sprite2D : public Object2D {
    public:
        /** @brief Register the sprite type in the supplied registry. */
        static auto _regist_method(core::reflect::ClassDB& db) -> void;

        /** @brief Construct an untextured sprite at the origin. */
        Sprite2D() = default;

        /** @brief Decode and share an image on the calling thread. A failed load preserves the previous texture. */
        auto set_texture(const std::filesystem::path& path) -> void;

        /** @brief Pin the current CPU image across concurrent replacements and graphics sessions. */
        auto get_texture() const -> std::shared_ptr<const resource::Texture> {
            std::lock_guard guard(texture_mutex);
            return texture;
        }

    private:
        mutable std::mutex texture_mutex;
        std::shared_ptr<const resource::Texture> texture;
    };
} // namespace gkit::scene
