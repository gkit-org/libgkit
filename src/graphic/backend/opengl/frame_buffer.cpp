#include "graphic/backend/opengl/frame_buffer.hpp"

#include "graphic/backend/opengl/render_buffer.hpp"
#include "graphic/backend/opengl/texture.hpp"

#include <utility>

#include <glad/gl.h>

namespace gkit::graphic::opengl {

    FrameBuffer::FrameBuffer(const std::shared_ptr<const Context>& context, int width, int height) :
        ContextResource(context), fb_width(width), fb_height(height) {
        glGenFramebuffers(1, &this->renderer_id);
    }

    FrameBuffer::~FrameBuffer() {
        if (renderer_id != 0 && has_current_context()) {
            glDeleteFramebuffers(1, &this->renderer_id);
            this->renderer_id = 0;
        }
    }

    namespace {
        class DrawBinding {
            GLint previous = 0;

        public:
            explicit DrawBinding(GLuint framebuffer) {
                glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &previous);
                glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffer);
            }
            ~DrawBinding() { glBindFramebuffer(GL_DRAW_FRAMEBUFFER, previous); }
        };

        auto validate_slot(int slot) -> void {
            GLint slots = 0;
            glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &slots);
            if (slot < 0 || slot >= slots) throw std::out_of_range("invalid color attachment slot");
        }
    } // namespace

    auto FrameBuffer::attach_color_texture(std::shared_ptr<const graphic::Texture> texture, int slot) -> void {
        require_context();
        if (!texture) throw std::invalid_argument("missing color attachment");
        const auto& native = require_resource<Texture>(*texture);
        if (native.get_width() != width() || native.get_height() != height())
            throw std::invalid_argument("color attachment dimensions must match the framebuffer");
        validate_slot(slot);
        color_attachments.insert_or_assign(slot, std::move(texture));
        DrawBinding binding(renderer_id);
        glFramebufferTexture2D(
            GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + slot, GL_TEXTURE_2D, native.get_renderer_id(), 0);
    }

    auto FrameBuffer::detach_color_texture(int slot) -> void {
        require_context();
        validate_slot(slot);
        DrawBinding binding(renderer_id);
        glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + slot, GL_TEXTURE_2D, 0, 0);
        color_attachments.erase(slot);
    }

    auto FrameBuffer::attach_depth_stencil(std::shared_ptr<const graphic::RenderBuffer> rbo) -> void {
        require_context();
        if (!rbo) throw std::invalid_argument("missing depth-stencil attachment");
        const auto& native = require_resource<RenderBuffer>(*rbo);
        if (native.width() != width() || native.height() != height())
            throw std::invalid_argument("depth-stencil attachment dimensions must match the framebuffer");
        DrawBinding binding(renderer_id);
        glFramebufferRenderbuffer(
            GL_DRAW_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, native.get_render_id());
        depth_stencil = std::move(rbo);
    }

    auto FrameBuffer::detach_depth_stencil() -> void {
        require_context();
        DrawBinding binding(renderer_id);
        glFramebufferRenderbuffer(GL_DRAW_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, 0);
        depth_stencil.reset();
    }

    auto FrameBuffer::check() -> void {
        require_context();
        DrawBinding binding(renderer_id);
        if (glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            throw std::runtime_error("framebuffer is incomplete");
    }

    auto FrameBuffer::set_viewport(int x, int y, int width, int height) -> void {
        require_context();
        glViewport(x, y, width, height);
    }

    auto FrameBuffer::set_viewport(int width, int height) -> void {
        require_context();
        glViewport(this->left_x, this->bottom_y, width, height);
    }

    auto FrameBuffer::set_viewport() -> void {
        require_context();
        glViewport(this->left_x, this->bottom_y, this->fb_width, this->fb_height);
    }

    auto FrameBuffer::bind() const -> void {
        require_context();
        glBindFramebuffer(GL_FRAMEBUFFER, this->renderer_id);
    }

    auto FrameBuffer::unbind() const -> void {
        require_context();
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    auto FrameBuffer::width() const -> int {
        return static_cast<int>(this->fb_width);
    }

    auto FrameBuffer::height() const -> int {
        return static_cast<int>(this->fb_height);
    }

} // namespace gkit::graphic::opengl
