#pragma once

#include "gkit/math/matrix3.hpp"
#include "gkit/math/matrix4.hpp"
#include "gkit/math/vector3.hpp"
#include "gkit/math/vector4.hpp"

#include <string>

/**
 * @brief Shader (frontend abstract interface)
 *
 * Unified interface for loading/compiling/binding shader programs.
 * Concrete backends (OpenGL/Vulkan) handle compilation, linking, and uniforms.
 */
namespace gkit::graphic {

    /**
	 * @brief Shader source split by stage
	 */
    struct ShaderProgramSource {
        std::string vertex_shader; // Vertex shader source code
        std::string fragment_shader; // Fragment shader source code
    };

    class Shader {
    public:
        Shader()                                 = default;
        Shader(const Shader&)                    = delete;
        auto operator=(const Shader&) -> Shader& = delete;

        Shader(Shader&&) noexcept                    = default;
        auto operator=(Shader&&) noexcept -> Shader& = default;

        virtual ~Shader() = default;

        /**
		 * @brief Bind the shader program to the current backend context
		 */
        virtual auto bind() const -> void = 0;

        /**
		 * @brief Unbind
		 */
        virtual auto unbind() const -> void = 0;

        /**
		 * @brief Whether the shader program compiled and linked successfully
		 *
		 * A shader created from a file whose source failed to compile/link is
		 * invalid; rendering with it is undefined, so the queue rejects it.
		 */
        [[nodiscard]] virtual auto is_valid() const -> bool = 0;

        // Uniform setters (implemented by backends, mapped to the concrete API)

        virtual auto set_uniform_1i(const std::string& name, int value) -> void                              = 0;
        virtual auto set_uniform_1f(const std::string& name, float value) -> void                            = 0;
        virtual auto set_uniform_4f(const std::string& name, float v0, float v1, float v2, float v3) -> void = 0;
        virtual auto set_uniform_vec_4f(const std::string& name, const math::Vector4& vec4) -> void          = 0;
        virtual auto set_uniform_vec_3f(const std::string& name, const math::Vector3& vec3) -> void          = 0;
        virtual auto set_uniform_mat_4f(const std::string& name, const math::Matrix4& mat4) -> void          = 0;
        virtual auto set_uniform_mat_3f(const std::string& name, const math::Matrix3& mat3) -> void          = 0;
        virtual auto set_uniform_1iv(const std::string& name, int count, const int* values) -> void          = 0;
    };

} // namespace gkit::graphic
