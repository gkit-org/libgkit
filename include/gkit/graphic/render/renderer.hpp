/** @file renderer.hpp
 * @brief Resource creation and draw submission for a windowed application.
 */
#pragma once

#include "gkit/graphic/render/render_object.hpp"
#include "gkit/graphic/render/viewport_rect.hpp"
#include "gkit/graphic/resource/frame_buffer.hpp"

#include <memory>
#include <optional>
#include <string>

namespace gkit::graphic {
    class Viewport;
    class Application;
    class RenderDevice;

    /**
     * @brief Renderer for the currently open window session.
     * @note Obtain it through Application::renderer() on the coordinator thread. Resource creation is
     * available while the window is open. Clear and draw calls are accepted only inside Application::render().
     */
    class Renderer {
    public:
        /** @brief Load a shader from a file. Throws std::runtime_error on compilation or linking failure. */
        [[nodiscard]] auto create_shader(const std::string& path) -> std::shared_ptr<Shader>;
        /** @brief Create an off-screen framebuffer with positive dimensions in pixels. */
        [[nodiscard]] auto create_frame_buffer(int width, int height) -> std::shared_ptr<FrameBuffer>;
        /** @brief Create an RGBA texture usable as a framebuffer color attachment. */
        [[nodiscard]] auto create_color_attachment(int width, int height) -> std::shared_ptr<Texture>;
        /** @brief Create a depth-stencil attachment with positive dimensions in pixels. */
        [[nodiscard]] auto create_render_buffer(int width, int height) -> std::shared_ptr<RenderBuffer>;

        /** @brief Clear the window framebuffer before executing this frame's queued draws. */
        auto clear(ClearFlags flags = ClearFlags::All) -> void;
        /**
         * @brief Enqueue a draw to the window.
         * @param object Geometry and material. The queue retains the object and captures its draw state by value.
         * @param viewport Optional viewport in pixels. The default covers the window drawable.
         * @throws std::invalid_argument For an empty object, invalid material, viewport or instance count.
         * @throws std::logic_error Outside render() or if the resources belong to an expired context.
         */
        auto draw(std::shared_ptr<RenderObject> object, std::optional<ViewportRect> viewport = std::nullopt) -> void;
        /**
         * @brief Enqueue a draw to an off-screen target. Off-screen draws execute before window draws.
         * @param target The retained framebuffer. Configure its attachments before queuing draws.
         * @param object Geometry and material.
         * @param viewport Optional viewport in pixels. The default covers the target.
         * @see draw()
         */
        auto draw_to(std::shared_ptr<const FrameBuffer> target,
                     std::shared_ptr<RenderObject> object,
                     std::optional<ViewportRect> viewport = std::nullopt) -> void;

    private:
        friend class Application;
        friend class SpritePass;
        explicit Renderer(std::shared_ptr<RenderDevice> device);
        ~Renderer();
        Renderer(const Renderer&)                    = delete;
        auto operator=(const Renderer&) -> Renderer& = delete;

        auto draw_sprites(Viewport& root, int width, int height, int pixel_width, int pixel_height) -> void;
        auto begin_frame() -> void;
        auto finish_frame(ViewportRect screen) -> void;
        auto discard_frame() noexcept -> void;
        auto require_context() const -> void;
        auto require_frame() const -> void;
        auto enqueue(std::shared_ptr<const FrameBuffer> target,
                     std::shared_ptr<RenderObject> object,
                     std::optional<ViewportRect> viewport,
                     std::optional<ViewportRect> clip = std::nullopt) -> void;

        struct Impl;
        std::unique_ptr<Impl> impl;
    };
} // namespace gkit::graphic
