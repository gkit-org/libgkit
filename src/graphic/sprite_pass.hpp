#pragma once

#include "gkit/graphic/render/renderer.hpp"
#include "gkit/resource/texture.hpp"

#include <map>

namespace gkit::graphic {
    class RenderDevice;

    /** @brief Coordinator-owned sprite resources for a single graphics session. */
    class SpritePass {
    public:
        /** @brief Map a positive logical window extent to clip space, independently of framebuffer pixel density. */
        static auto logical_projection(int width, int height) -> math::Matrix3;

        /** @brief Map an already intersected top-left logical clip to a bottom-left pixel scissor.
         * Round shared boundaries identically, including at fractional display scales. */
        static auto pixel_clip(ViewportRect clip, int width, int height, int pixel_width, int pixel_height)
            -> ViewportRect;
        auto submit(Viewport& root,
                    int width,
                    int height,
                    int pixel_width,
                    int pixel_height,
                    RenderDevice& device,
                    Renderer& renderer) -> void;

    private:
        auto mesh(const std::shared_ptr<const resource::Texture>& image, RenderDevice& device)
            -> std::shared_ptr<RenderObject>;

        std::shared_ptr<Shader> shader;
        std::map<std::weak_ptr<const resource::Texture>, std::shared_ptr<RenderObject>, std::owner_less<>> images;
    };
} // namespace gkit::graphic
