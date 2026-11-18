#include "gkit/graphic/render/renderer.hpp"

#include "graphic/backend/opengl/device.hpp"
#include "graphic/backend/opengl/frame_buffer.hpp"
#include "graphic/backend/opengl/shader.hpp"
#include "graphic/backend/opengl/texture.hpp"
#include "graphic/render_queue.hpp"

#include <glad/gl.h>

namespace gkit::graphic {
    namespace {
        auto require_positive_dimensions(int width, int height) -> void {
            if (width <= 0 || height <= 0) throw std::invalid_argument("graphics dimensions must be positive");
        }
    } // namespace

    struct Renderer::Impl {
        std::shared_ptr<const opengl::Context> context;
        opengl::Device device;
        RenderQueue queue;
        bool recording = false;

        explicit Impl(std::shared_ptr<const opengl::Context> context) : context(std::move(context)), device(this->context) {}
    };

    Renderer::Renderer(std::shared_ptr<const opengl::Context> context) : impl(std::make_unique<Impl>(std::move(context))) {}
    Renderer::~Renderer() = default;

    auto Renderer::require_context() const -> void {
        if (!impl->context->is_current()) throw std::logic_error("the renderer's OpenGL context is not current");
    }
    auto Renderer::require_frame() const -> void {
        require_context();
        if (!impl->recording) throw std::logic_error("draw and clear require Application::render()");
    }

    auto Renderer::create_shader(const std::string& path) -> std::shared_ptr<Shader> {
        require_context();
        auto shader = impl->device.create_shader(path);
        if (!shader->is_valid()) throw std::runtime_error("failed to load shader: " + path);
        return std::shared_ptr<Shader>(std::move(shader));
    }
    auto Renderer::create_frame_buffer(int width, int height) -> std::shared_ptr<FrameBuffer> {
        require_context();
        require_positive_dimensions(width, height);
        return std::shared_ptr<FrameBuffer>(impl->device.create_frame_buffer(width, height));
    }
    auto Renderer::create_color_attachment(int width, int height) -> std::shared_ptr<Texture> {
        require_context();
        require_positive_dimensions(width, height);
        return std::shared_ptr<Texture>(impl->device.create_color_attachment(width, height));
    }
    auto Renderer::create_render_buffer(int width, int height) -> std::shared_ptr<RenderBuffer> {
        require_context();
        require_positive_dimensions(width, height);
        return std::shared_ptr<RenderBuffer>(impl->device.create_render_buffer(width, height));
    }

    auto Renderer::begin_frame() -> void {
        require_context();
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        impl->recording = true;
    }
    auto Renderer::finish_frame(Viewport screen) -> void {
        require_frame();
        impl->recording = false;
        impl->queue.flush(impl->device, impl->context, screen);
    }
    auto Renderer::discard_frame() noexcept -> void {
        impl->recording = false;
        impl->queue.clear();
        if (impl->context->is_current()) glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
    auto Renderer::clear(ClearFlags flags) -> void {
        require_frame();
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        impl->device.clear(flags);
    }

    auto Renderer::draw(std::shared_ptr<RenderObject> object, std::optional<Viewport> viewport) -> void {
        enqueue({}, std::move(object), viewport);
    }
    auto Renderer::draw_to(std::shared_ptr<const FrameBuffer> target,
                           std::shared_ptr<RenderObject> object,
                           std::optional<Viewport> viewport) -> void {
        if (!target) throw std::invalid_argument("missing off-screen framebuffer");
        enqueue(std::move(target), std::move(object), viewport);
    }
    auto Renderer::enqueue(std::shared_ptr<const FrameBuffer> target,
                           std::shared_ptr<RenderObject> object,
                           std::optional<Viewport> viewport) -> void {
        require_frame();
        if (!object) throw std::invalid_argument("missing render object");
        if (viewport) require_positive_dimensions(viewport->width, viewport->height);
        if (target) opengl::require_resource<opengl::FrameBuffer>(*target);
        if (object->instance_count == 0) throw std::invalid_argument("instance count must be positive");
        const auto& material = object->material;
        const auto* shader = material.get_shader();
        if (!shader) throw std::invalid_argument("render object requires a shader");
        opengl::require_resource<opengl::Shader>(*shader);
        if (!shader->is_valid()) throw std::invalid_argument("render object requires a valid shader");
        for (std::uint32_t slot = 0; slot < material.get_texture_count(); ++slot) {
            opengl::require_resource<opengl::Texture>(*material.get_texture(slot));
        }
        impl->queue.submit({.target = std::move(target),
                            .object = object,
                            .material = material,
                            .viewport = viewport,
                            .state = object->state,
                            .instance_count = object->instance_count,
                            .transparent = object->transparent,
                            .depth_key = object->depth_key,
                            .clear = object->clear,
                            .clear_flags = object->clear_flags});
    }
} // namespace gkit::graphic
