#pragma once

namespace gkit::graphic {

    /**
	 * @brief Renderbuffer (frontend abstract interface)
	 *
	 * Off-screen renderbuffer (OpenGL renderbuffer) that cannot be sampled
	 * directly by shaders; typically used as the depth/stencil attachment of
	 * a framebuffer. Not part of the Buffer family (it is a render-target
	 * attachment rather than a data buffer).
	 */
    class RenderBuffer {
    public:
        RenderBuffer()                                       = default;
        RenderBuffer(const RenderBuffer&)                    = delete;
        auto operator=(const RenderBuffer&) -> RenderBuffer& = delete;

        RenderBuffer(RenderBuffer&&) noexcept                    = default;
        auto operator=(RenderBuffer&&) noexcept -> RenderBuffer& = default;

        virtual ~RenderBuffer() = default;

        /**
		 * @brief Bind to the current backend context
		 */
        virtual auto bind() const -> void = 0;

        /**
		 * @brief Unbind
		 */
        virtual auto unbind() const -> void = 0;
    };

} // namespace gkit::graphic
