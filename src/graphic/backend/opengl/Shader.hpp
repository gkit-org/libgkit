#pragma once

#include "gkit/graphic/resource/Shader.hpp"

#include <cstdint>
#include <string>
#include <unordered_map>

/**
 * @brief OpenGL backend shader program
 *
 * Inherits frontend `graphic::Shader`; implements GL shader loading,
 * compilation, linking, and uniform operations.
 */
namespace gkit::graphic::opengl {

    class Shader final : public graphic::Shader {
        friend class Device;

    public:
        Shader(const Shader&)                    = delete;
        auto operator=(const Shader&) -> Shader& = delete;

        Shader(Shader&& other) noexcept;
        auto operator=(Shader&& other) noexcept -> Shader&;

        ~Shader() override;

        auto bind() const -> void override;
        auto unbind() const -> void override;

        [[nodiscard]] auto is_valid() const -> bool override;

        auto set_uniform_1i(const std::string& name, int value) -> void override;
        auto set_uniform_1f(const std::string& name, float value) -> void override;
        auto set_uniform_4f(const std::string& name, float v0, float v1, float v2, float v3) -> void override;
        auto set_uniform_vec_4f(const std::string& name, const gkit::math::Vector4& vec4) -> void override;
        auto set_uniform_vec_3f(const std::string& name, const gkit::math::Vector3& vec3) -> void override;
        auto set_uniform_mat_4f(const std::string& name, const gkit::math::Matrix4& mat4) -> void override;
        auto set_uniform_mat_3f(const std::string& name, const gkit::math::Matrix3& mat3) -> void override;
        auto set_uniform_1iv(const std::string& name, int count, const int* values) -> void override;

    private:
        /**
		 * @brief Construct a shader from a file (only Device may create one)
		 * @param filepath path to the shader source file
		 */
        explicit Shader(const std::string& filepath);

        auto parse_shader(const std::string& file_path) -> graphic::ShaderProgramSource;
        auto compile_shader(uint32_t type, const std::string& source) -> uint32_t;
        auto create_shader(const std::string& vertex_shader, const std::string& fragment_shader) -> uint32_t;
        auto get_uniform_location(const std::string& name) -> int;

    private:
        uint32_t renderer_id = 0; // GL shader program handle
        std::string file_path; // shader file path
        std::unordered_map<std::string, int> uniform_location_cache; // uniform location cache
    };

} // namespace gkit::graphic::opengl
