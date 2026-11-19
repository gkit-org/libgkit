#include "graphic/sprite_pass.hpp"

#include "core/node_internal.hpp"
#include "gkit/graphic/viewport.hpp"
#include "gkit/scene/2d/sprite2d.hpp"
#include "graphic/render_device.hpp"

#include <algorithm>

namespace gkit::graphic {
    namespace {
        struct Region {
            std::int64_t x, y;
            int width, height;
            ViewportRect visible;

            [[nodiscard]] auto child(const Viewport& node) const -> std::optional<Region> {
                const auto rect    = node.get_rect().value_or(ViewportRect{0, 0, width, height});
                const auto child_x = x + rect.x, child_y = y + rect.y;
                const auto left   = std::max<std::int64_t>(visible.x, child_x);
                const auto top    = std::max<std::int64_t>(visible.y, child_y);
                const auto right  = std::min<std::int64_t>(visible.x + visible.width, child_x + rect.width);
                const auto bottom = std::min<std::int64_t>(visible.y + visible.height, child_y + rect.height);
                if (left >= right || top >= bottom) return std::nullopt;
                return Region{child_x,
                              child_y,
                              rect.width,
                              rect.height,
                              {static_cast<int>(left),
                               static_cast<int>(top),
                               static_cast<int>(right - left),
                               static_cast<int>(bottom - top)}};
            }
        };
    } // namespace

    auto SpritePass::pixel_clip(ViewportRect clip, int width, int height, int pixel_width, int pixel_height)
        -> ViewportRect {
        auto edge = [](std::int64_t value, int logical, int pixels) -> int {
            return static_cast<int>((value * pixels + logical / 2) / logical);
        };
        const int left   = edge(clip.x, width, pixel_width);
        const int bottom = edge(clip.y + clip.height, height, pixel_height);
        return {left,
                pixel_height - bottom,
                edge(clip.x + clip.width, width, pixel_width) - left,
                bottom - edge(clip.y, height, pixel_height)};
    }


    auto SpritePass::logical_projection(int width, int height) -> math::Matrix3 {
        auto projection    = math::Matrix3::scaling(2.f / width, -2.f / height, 1.f);
        projection.m[2][0] = -1.f;
        projection.m[2][1] = 1.f;
        return projection;
    }


    auto SpritePass::mesh(const std::shared_ptr<const resource::Texture>& image, RenderDevice& device)
        -> std::shared_ptr<RenderObject> {
        if (auto it = images.find(image); it != images.end()) return it->second;
        if (!shader) shader = device.create_sprite_shader();
        Material material;
        material.set_shader(shader);
        material.set_texture(device.upload_texture(*image));
        material.set_uniform("image", 0);
        VertexBufferLayout layout;
        layout.push<float>(2);
        auto object = std::make_shared<RenderObject>(
            std::vector<float>{0, 0, 1, 0, 1, 1, 0, 1}, std::vector<std::uint32_t>{0, 1, 2, 0, 2, 3}, layout, material);
        object->transparent = true;
        object->state.blend = {.enabled   = true,
                               .src_rgb   = BlendFunc::SrcAlpha,
                               .dst_rgb   = BlendFunc::OneMinusSrcAlpha,
                               .src_alpha = BlendFunc::One,
                               .dst_alpha = BlendFunc::OneMinusSrcAlpha};
        images.emplace(image, object);
        return object;
    }


    auto SpritePass::submit(Viewport& root,
                            int width,
                            int height,
                            int pixel_width,
                            int pixel_height,
                            RenderDevice& device,
                            Renderer& renderer) -> void {
        if (width <= 0 || height <= 0) return;
        std::erase_if(images, [](const auto& entry) -> bool {
            return entry.first.expired();
        });
        const auto projection = logical_projection(width, height);
        struct Visit {
            core::detail::NodeState* state;
            math::Matrix3 transform;
            Region region;
            std::optional<ViewportRect> clip;
        };
        // Frame barrier: edits remain queued and all physics/logic workers have joined.
        std::vector<Visit> pending{
            {&core::detail::NodeState::of(root), projection, {0, 0, width, height, {0, 0, width, height}}, {}}};
        while (!pending.empty()) {
            auto [state, transform, region, clip] = pending.back();
            pending.pop_back();
            if (auto* view = dynamic_cast<Viewport*>(&state->node); view && view != &root) {
                auto child = region.child(*view);
                if (!child) continue;
                region = *child;
                clip   = pixel_clip(region.visible, width, height, pixel_width, pixel_height);
                if (!clip->width || !clip->height) continue;
                auto origin    = math::Matrix3::identity();
                origin.m[2][0] = static_cast<float>(region.x);
                origin.m[2][1] = static_cast<float>(region.y);
                transform      = projection * origin;
            }
            if (auto* object = dynamic_cast<scene::Object2D*>(&state->node)) {
                auto local = object->get_transform();
                auto scale = local.get_scale();
                auto matrix =
                    math::Matrix3::rotation_z(local.get_rotation()) * math::Matrix3::scaling(scale.x, scale.y, 1.f);
                matrix.m[2][0] = local.get_position().x;
                matrix.m[2][1] = local.get_position().y;
                transform      = transform * matrix;
                if (auto* sprite = dynamic_cast<scene::Sprite2D*>(object)) {
                    if (auto image = sprite->get_texture()) {
                        auto object_mesh = mesh(image, device);
                        object_mesh->material.set_uniform(
                            "transform", transform * math::Matrix3::scaling(image->width(), image->height(), 1.f));
                        renderer.enqueue({}, object_mesh, {}, clip);
                    }
                }
            }
            for (auto* child : state->descendants() | std::views::reverse)
                pending.push_back({child, transform, region, clip});
        }
    }
} // namespace gkit::graphic
