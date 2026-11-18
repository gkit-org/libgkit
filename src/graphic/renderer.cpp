#include "gkit/graphic/render/renderer.hpp"

#include "graphic/render_device.hpp"
#include "graphic/render_queue.hpp"
#include "graphic/sprite_pass.hpp"

#include <stdexcept>

namespace gkit::graphic {
    namespace {
        auto require_positive_dimensions(int width, int height) -> void {
            if (width <= 0 || height <= 0) throw std::invalid_argument("graphics dimensions must be positive");
        }
    } // namespace

    struct Renderer::Impl {
        std::shared_ptr<RenderDevice> device;
        RenderQueue queue;
        SpritePass sprites;
        bool recording = false;
    };

    Renderer::Renderer(std::shared_ptr<RenderDevice> device) : impl(std::make_unique<Impl>()) {
        impl->device = std::move(device);
    }
    Renderer::~Renderer() = default;

    auto Renderer::require_context() const -> void {
        if (!impl->device->is_current()) throw std::logic_error("the graphics session is not current");
    }
    auto Renderer::require_frame() const -> void {
        require_context();
        if (!impl->recording) throw std::logic_error("draw and clear require Application::render()");
    }

    auto Renderer::create_shader(const std::string& path) -> std::shared_ptr<Shader> {
        require_context();
        auto shader = impl->device->create_shader(path);
        if (!shader->is_valid()) throw std::runtime_error("failed to load shader: " + path);
        return std::shared_ptr<Shader>(std::move(shader));
    }
    auto Renderer::create_frame_buffer(int width, int height) -> std::shared_ptr<FrameBuffer> {
        require_context();
        require_positive_dimensions(width, height);
        return std::shared_ptr<FrameBuffer>(impl->device->create_frame_buffer(width, height));
    }
    auto Renderer::create_color_attachment(int width, int height) -> std::shared_ptr<Texture> {
        require_context();
        require_positive_dimensions(width, height);
        return std::shared_ptr<Texture>(impl->device->create_color_attachment(width, height));
    }
    auto Renderer::create_render_buffer(int width, int height) -> std::shared_ptr<RenderBuffer> {
        require_context();
        require_positive_dimensions(width, height);
        return std::shared_ptr<RenderBuffer>(impl->device->create_render_buffer(width, height));
    }

    auto Renderer::draw_sprites(Viewport& root, int width, int height, int pixel_width, int pixel_height) -> void {
        impl->sprites.submit(root, width, height, pixel_width, pixel_height, *impl->device, *this);
    }
    auto Renderer::begin_frame() -> void {
        require_context();
        impl->device->bind_window_target();
        impl->recording = true;
    }
    auto Renderer::finish_frame(ViewportRect screen) -> void {
        require_frame();
        impl->recording = false;
        impl->queue.flush(*impl->device, impl->device, screen);
        impl->device->present();
    }
    auto Renderer::discard_frame() noexcept -> void {
        impl->recording = false;
        impl->queue.clear();
        if (impl->device->is_current()) {
            impl->device->set_clip(std::nullopt);
            impl->device->bind_window_target();
        }
    }
    auto Renderer::clear(ClearFlags flags) -> void {
        require_frame();
        impl->device->clear_window(flags);
    }

    auto Renderer::draw(std::shared_ptr<RenderObject> object, std::optional<ViewportRect> viewport) -> void {
        enqueue({}, std::move(object), viewport);
    }
    auto Renderer::draw_to(std::shared_ptr<const FrameBuffer> target,
                           std::shared_ptr<RenderObject> object,
                           std::optional<ViewportRect> viewport) -> void {
        if (!target) throw std::invalid_argument("missing off-screen framebuffer");
        enqueue(std::move(target), std::move(object), viewport);
    }
    auto Renderer::enqueue(std::shared_ptr<const FrameBuffer> target,
                           std::shared_ptr<RenderObject> object,
                           std::optional<ViewportRect> viewport,
                           std::optional<ViewportRect> clip) -> void {
        require_frame();
        if (!object) throw std::invalid_argument("missing render object");
        if (viewport) require_positive_dimensions(viewport->width, viewport->height);
        if (object->instance_count == 0) throw std::invalid_argument("instance count must be positive");
        const auto& material = object->material;
        impl->device->validate_submission(material, target.get());
        impl->queue.submit({.target         = std::move(target),
                            .object         = object,
                            .material       = material,
                            .viewport       = viewport,
                            .clip           = clip,
                            .state          = object->state,
                            .instance_count = object->instance_count,
                            .transparent    = object->transparent,
                            .depth_key      = object->depth_key,
                            .clear          = object->clear,
                            .clear_flags    = object->clear_flags});
    }
} // namespace gkit::graphic
