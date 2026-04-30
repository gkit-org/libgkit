#pragma once

#include <string>
#include <unordered_map>
#include <cstdint>

namespace gkit::graphic{
	/**
 	 * @brief Structure holding parsed shader source code
	 */
	struct ShaderProgramSource {
		std::string vertexShader;    ///< Vertex shader source code
		std::string fragmentShader;   ///< Fragment shader source code (note: typo in original)
	};

	/**
	 * @brief Shader wrapper for OpenGL shader programs
	 *
	 * Provides functionality to load, compile, and manage shader programs
	 * including uniform variable manipulation.
	 */
	class Shader {
	public:
		Shader(const Shader&) = delete;
		Shader& operator=(const Shader&) = delete;

		/** @brief Move constructor - transfers ownership of GL shader program
		 *  @param other Source object to move from (will be invalidated)
		 */
		Shader(Shader&& other) noexcept;

		/** @brief Move assignment - transfers ownership of GL shader program
		 *  @param other Source object to move from (will be invalidated)
		 *  @note Releases any existing GL shader program before taking ownership
		 */
		auto operator=(Shader&& other) noexcept -> Shader&;

	private:
		uint32_t m_RendererID;                                  ///< OpenGL shader program ID
		std::string m_FilePath;                                   ///< Path to the shader file
		std::unordered_map<std::string, int> m_UniformLocationCach; ///< Cache for uniform locations
	public:
		/**
		 * @brief Construct a shader from a file
		 * @param filepath Path to the shader source file
		 */
		Shader(const std::string& filepath);

		/**
		 * @brief Destructor - deletes the shader program
		 */
		~Shader();

		/**
		 * @brief Bind this shader program to the current OpenGL context
		 */
		auto bind() const -> void;

		/**
		 * @brief Unbind this shader program from the current OpenGL context
		 */
		auto unbind() const -> void;

		// Uniform setters

		/**
		 * @brief Set an integer uniform variable
		 * @param name Name of the uniform
		 * @param value Integer value to set
		 */
		auto set_uniform_1i(const std::string& name, int value) -> void;

		/**
		 * @brief Set a float uniform variable
		 * @param name Name of the uniform
		 * @param value Float value to set
		 */
		auto set_uniform_1f(const std::string& name, float value) -> void;

		/**
		 * @brief Set a 4-component float uniform variable
		 * @param name Name of the uniform
		 * @param v0 First component
		 * @param v1 Second component
		 * @param f2 Third component
		 * @param f3 Fourth component
		 */
		auto set_uniform_4f(const std::string& name, float v0, float v1, float f2, float f3) -> void;

		/**
		 * @brief Set a 4-component float vector uniform
		 * @param name Name of the uniform
		 * @param vector4 Pointer to 4-component float array
		 */
		auto set_uniform_vec_4f(const std::string& name, const float* vector4) -> void;

		/**
		 * @brief Set a 3-component float vector uniform
		 * @param name Name of the uniform
		 * @param vector3 Pointer to 3-component float array
		 */
		auto set_uniform_vec_3f(const std::string& name, const float* vector3) -> void;

		/**
		 * @brief Set a 4x4 float matrix uniform
		 * @param name Name of the uniform
		 * @param matrix Pointer to 16-element float array (column-major)
		 */
		auto set_uniform_mat_4f(const std::string& name, const float* matrix) -> void;

		/**
		 * @brief Set a 3x3 float matrix uniform
		 * @param name Name of the uniform
		 * @param matrix Pointer to 9-element float array (column-major)
		 */
		auto set_uniform_mat_3f(const std::string& name, const float* matrix) -> void;

		/**
		 * @brief Set an array of integer uniform variables
		 * @param name Name of the uniform
		 * @param sz Number of elements
		 * @param ind Array of integer values
		 */
		auto set_uniform_1iv(const std::string& name, const int sz, const int* ind) -> void;

	private:
		/**
		 * @brief Parse shader source from file
		 * @param filePath Path to the shader file
		 * @return Parsed shader source structure
		 */
		auto parse_shader(const std::string& filePath) -> ShaderProgramSource;

		/**
		 * @brief Compile a shader of the specified type
		 * @param type Shader type (GL_VERTEX_SHADER or GL_FRAGMENT_SHADER)
		 * @param source Shader source code
		 * @return Compiled shader ID
		 */
		auto compile_shader(uint32_t type, const std::string& source) -> uint32_t;

		/**
		 * @brief Create a shader program from vertex and fragment shaders
		 * @param vertexShader Vertex shader source
		 * @param fragmentShader Fragment shader source
		 * @return Linked shader program ID
		 */
		auto create_shader(const std::string& vertexShader, const std::string& fragmentShader) -> uint32_t;

		/**
		 * @brief Get the location of a uniform variable
		 * @param name Name of the uniform
		 * @return Location ID, or -1 if not found
		 */
		auto get_uniform_location(const std::string& name) -> int;

	};
}