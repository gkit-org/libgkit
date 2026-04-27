#include "gkit/graphic/opengl/StateManager.hpp"

namespace gkit::graphic::opengl {

	auto StateManager::Get() -> StateManager& {
		static StateManager instance;
		return instance;
	}

	auto StateManager::SetDepthTest(bool enable) -> void {
		if (m_DepthState.enabled != enable) {
			m_DepthState.enabled = enable;
			m_DirtyFlags |= DIRTY_DEPTH;
		}
	}

	auto StateManager::SetDepthFunc(CompareFunc func) -> void {
		if (m_DepthState.compareFunc != func) {
			m_DepthState.compareFunc = func;
			m_DirtyFlags |= DIRTY_DEPTH;
		}
	}

	auto StateManager::SetDepthMask(bool write) -> void {
		if (m_DepthState.writeMask != write) {
			m_DepthState.writeMask = write;
			m_DirtyFlags |= DIRTY_DEPTH;
		}
	}

	auto StateManager::SetBlend(bool enable) -> void {
		if (m_BlendState.enabled != enable) {
			m_BlendState.enabled = enable;
			m_DirtyFlags |= DIRTY_BLEND;
		}
	}

	auto StateManager::SetBlendFunc(BlendFunc srcRGB, BlendFunc dstRGB, BlendFunc srcAlpha, BlendFunc dstAlpha) -> void {
		if (m_BlendState.srcRGB != srcRGB || m_BlendState.dstRGB != dstRGB ||
			m_BlendState.srcAlpha != srcAlpha || m_BlendState.dstAlpha != dstAlpha) {
			m_BlendState.srcRGB = srcRGB;
			m_BlendState.dstRGB = dstRGB;
			m_BlendState.srcAlpha = srcAlpha;
			m_BlendState.dstAlpha = dstAlpha;
			m_DirtyFlags |= DIRTY_BLEND;
		}
	}

	auto StateManager::SetBlendEquation(BlendEquation equation) -> void {
		if (m_BlendState.equation != equation) {
			m_BlendState.equation = equation;
			m_DirtyFlags |= DIRTY_BLEND;
		}
	}

	auto StateManager::SetCullFace(bool enable) -> void {
		if (m_CullFaceState.enabled != enable) {
			m_CullFaceState.enabled = enable;
			m_DirtyFlags |= DIRTY_CULL;
		}
	}

	auto StateManager::SetCullFaceMode(CullFaceMode mode) -> void {
		if (m_CullFaceState.mode != mode) {
			m_CullFaceState.mode = mode;
			m_DirtyFlags |= DIRTY_CULL;
		}
	}

	auto StateManager::SetFrontFace(FrontFace frontFace) -> void {
		if (m_CullFaceState.frontFace != frontFace) {
			m_CullFaceState.frontFace = frontFace;
			m_DirtyFlags |= DIRTY_CULL;
		}
	}

	auto StateManager::SetStencilTest(bool enable) -> void {
		if (m_StencilState.enabled != enable) {
			m_StencilState.enabled = enable;
			m_DirtyFlags |= DIRTY_STENCIL;
		}
	}

	auto StateManager::SetStencil(CompareFunc func, uint32_t ref, uint32_t mask) -> void {
		if (m_StencilState.compareFunc != func || m_StencilState.ref != ref || m_StencilState.readMask != mask) {
			m_StencilState.compareFunc = func;
			m_StencilState.ref = ref;
			m_StencilState.readMask = mask;
			m_DirtyFlags |= DIRTY_STENCIL;
		}
	}

	auto StateManager::SetStencilOp(StencilOp fail, StencilOp zFail, StencilOp zPass) -> void {
		if (m_StencilState.fail != fail || m_StencilState.zFail != zFail || m_StencilState.zPass != zPass) {
			m_StencilState.fail = fail;
			m_StencilState.zFail = zFail;
			m_StencilState.zPass = zPass;
			m_DirtyFlags |= DIRTY_STENCIL;
		}
	}

	auto StateManager::SetStencilMask(uint32_t mask) -> void {
		if (m_StencilState.writeMask != mask) {
			m_StencilState.writeMask = mask;
			m_DirtyFlags |= DIRTY_STENCIL;
		}
	}

	auto StateManager::Apply() -> void {
		if (m_DirtyFlags & DIRTY_DEPTH) {
			ApplyDepthState();
		}
		if (m_DirtyFlags & DIRTY_BLEND) {
			ApplyBlendState();
		}
		if (m_DirtyFlags & DIRTY_CULL) {
			ApplyCullFaceState();
		}
		if (m_DirtyFlags & DIRTY_STENCIL) {
			ApplyStencilState();
		}
		m_DirtyFlags = 0;
	}

	auto StateManager::ForceApplyAll() -> void {
		m_DirtyFlags = DIRTY_DEPTH | DIRTY_BLEND | DIRTY_CULL | DIRTY_STENCIL;
		Apply();
	}

	auto StateManager::ApplyDepthState() -> void {
		if (m_DepthState.enabled) {
			glEnable(GL_DEPTH_TEST);
		} else {
			glDisable(GL_DEPTH_TEST);
		}
		glDepthFunc(static_cast<GLenum>(m_DepthState.compareFunc));
		glDepthMask(m_DepthState.writeMask ? GL_TRUE : GL_FALSE);
	}

	auto StateManager::ApplyBlendState() -> void {
		if (m_BlendState.enabled) {
			glEnable(GL_BLEND);
		} else {
			glDisable(GL_BLEND);
		}
		glBlendFuncSeparate(
			static_cast<GLenum>(m_BlendState.srcRGB), static_cast<GLenum>(m_BlendState.dstRGB),
			static_cast<GLenum>(m_BlendState.srcAlpha), static_cast<GLenum>(m_BlendState.dstAlpha));
		glBlendEquation(static_cast<GLenum>(m_BlendState.equation));
	}

	auto StateManager::ApplyCullFaceState() -> void {
		if (m_CullFaceState.enabled) {
			glEnable(GL_CULL_FACE);
		} else {
			glDisable(GL_CULL_FACE);
		}
		glCullFace(static_cast<GLenum>(m_CullFaceState.mode));
		glFrontFace(static_cast<GLenum>(m_CullFaceState.frontFace));
	}

	auto StateManager::ApplyStencilState() -> void {
		if (m_StencilState.enabled) {
			glEnable(GL_STENCIL_TEST);
		} else {
			glDisable(GL_STENCIL_TEST);
		}
		glStencilFunc(
			static_cast<GLenum>(m_StencilState.compareFunc),
			m_StencilState.ref,
			m_StencilState.readMask);
		glStencilOp(
			static_cast<GLenum>(m_StencilState.fail),
			static_cast<GLenum>(m_StencilState.zFail),
			static_cast<GLenum>(m_StencilState.zPass));
		glStencilMask(m_StencilState.writeMask);
	}

	auto StateManager::GetDepthState() const -> const DepthState& {
		return m_DepthState;
	}

	auto StateManager::GetBlendState() const -> const BlendState& {
		return m_BlendState;
	}

	auto StateManager::GetCullFaceState() const -> const CullFaceState& {
		return m_CullFaceState;
	}

	auto StateManager::GetStencilState() const -> const StencilState& {
		return m_StencilState;
	}

} // namespace gkit::graphic::opengl
