#include "gkit/graphic/render/RenderQueue.hpp"

#include "gkit/graphic/render/RenderObject.hpp"

#include <algorithm>
#include <variant>

namespace gkit::graphic {

    namespace {

        /// @brief Bind a texture slot to the shader sampler unit
        auto bind_textures(const Material& material) -> void {
            for (uint32_t i = 0; i < material.texture_count && i < MAX_TEXTURE_SLOTS; ++i) {
                if (material.textures[i] != nullptr) {
                    material.textures[i]->bind(i);
                }
            }
        }

        /// @brief Apply a single uniform value through the shader's set_uniform_*
        auto apply_uniform_value(Shader& shader, const std::string& name, const UniformValue& value) -> void {
            std::visit(
                [&](const auto& v) {
                    using T = std::decay_t<decltype(v)>;
                    if constexpr (std::is_same_v<T, int>) {
                        shader.set_uniform_1i(name, v);
                    } else if constexpr (std::is_same_v<T, float>) {
                        shader.set_uniform_1f(name, v);
                    } else if constexpr (std::is_same_v<T, math::Vector4>) {
                        shader.set_uniform_vec_4f(name, v);
                    } else if constexpr (std::is_same_v<T, math::Vector3>) {
                        shader.set_uniform_vec_3f(name, v);
                    } else if constexpr (std::is_same_v<T, math::Matrix4>) {
                        shader.set_uniform_mat_4f(name, v);
                    } else if constexpr (std::is_same_v<T, math::Matrix3>) {
                        shader.set_uniform_mat_3f(name, v);
                    }
                },
                value);
        }

        /// @brief Apply material uniforms (simple path)
        auto apply_uniforms(const Material& material) -> void {
            if (material.shader == nullptr) {
                return;
            }
            for (const auto& [name, value] : material.uniforms.values) {
                apply_uniform_value(*material.shader, name, value);
            }
            // TODO(graphic): upload material.ubo via a UniformBuffer backend once implemented.
        }

        /// @brief Sort comparator: framebuffer commands first, then by state/transparency
        auto sort_key(const RenderCommand& cmd) -> uint64_t {
            // Render targets (FBO) must be drawn before screen commands, otherwise
            // post-processing cannot sample the FBO attachment. So target=null (screen)
            // sorts after any non-null target. Then group by state (reduce switches).
            const bool blend_enabled   = cmd.state.blend.enabled;
            const uint64_t target_rank = (cmd.target != nullptr) ? 0 : 1; // FBO before screen
            return (target_rank << 56) | (static_cast<uint64_t>(blend_enabled) << 48) |
                   (static_cast<uint64_t>(cmd.transparent) << 40);
        }

    } // namespace

    auto RenderQueue::flush(RenderDevice& device) -> void {
        // Sort: opaque front-to-back, transparent back-to-front; group state/shader.
        std::stable_sort(
            this->commands.begin(), this->commands.end(), [](const RenderCommand& a, const RenderCommand& b) {
                if (sort_key(a) != sort_key(b)) {
                    return sort_key(a) < sort_key(b);
                }
                // Within the same transparency class: opaque nearer-first, transparent farther-first.
                const float key_a = a.transparent ? -a.depth_key : a.depth_key;
                const float key_b = b.transparent ? -b.depth_key : b.depth_key;
                return key_a < key_b;
            });

        const FrameBuffer* last_target = nullptr;
        for (const auto& cmd : this->commands) {
            if (cmd.object == nullptr) {
                continue;
            }
            const Material& material = cmd.object->material;

            // Switch render target: unbind the previous FBO (reverting to screen)
            // before binding a different target. target=null means screen, reached
            // by unbinding the previous FBO.
            if (cmd.target != last_target) {
                if (last_target != nullptr) {
                    last_target->unbind();
                }
                last_target = cmd.target;
                if (cmd.target != nullptr) {
                    cmd.target->bind();
                }
            }

            // GL viewport is global state; each command sets its own viewport,
            // defaulting to the render target size when none was specified.
            const Viewport vp = cmd.viewport.value_or(
                cmd.target != nullptr ? Viewport{.x=0, .y=0, .width=cmd.target->width(), .height=cmd.target->height()}
                                      : Viewport{.x=0, .y=0, .width=static_cast<int>(SCR_WIDTH), .height=static_cast<int>(SCR_HEIGHT)});
            device.set_viewport(vp);

            // Clear the currently bound target (FBO or screen) if the command asks for it.
            if (cmd.clear) {
                device.clear(cmd.clear_flags);
            }

            device.apply_state(cmd.state);

            // Lazily upload geometry and bind shader/textures/uniforms.
            // Shader validity is enforced at enqueue time (Renderer::draw), so the
            // program is guaranteed non-null and valid here.
            const auto& vao = cmd.object->ensure_uploaded(device);
            const auto& ibo = cmd.object->index_buffer();
            material.shader->bind();
            bind_textures(material);
            apply_uniforms(material);

            if (cmd.instance_count > 1) {
                device.draw_instance(vao, ibo, *material.shader, cmd.instance_count);
            } else {
                device.draw(vao, ibo, *material.shader);
            }
        }

        // End of frame: leave the default framebuffer bound (screen).
        if (last_target != nullptr) {
            last_target->unbind();
        }
        this->commands.clear();
    }

} // namespace gkit::graphic
