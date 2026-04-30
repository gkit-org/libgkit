#include "gkit/graphic/Shader.hpp"
#include "gkit/utils/log.hpp"

#include <glad/gl.h>

#include <sstream>
#include <fstream>

gkit::graphic::Shader::Shader(const std::string& filepath)
	: m_FilePath(filepath), m_RendererID(0) {
    ShaderProgramSource source = parse_shader(filepath);
    m_RendererID = create_shader(source.vertexShader, source.fragmentShader);
}

gkit::graphic::Shader::~Shader() {
    glDeleteProgram(m_RendererID);
}

gkit::graphic::Shader::Shader(Shader&& other) noexcept
    : m_RendererID(other.m_RendererID)
    , m_FilePath(std::move(other.m_FilePath))
    , m_UniformLocationCach(std::move(other.m_UniformLocationCach)) {
    other.m_RendererID = 0;
}

auto gkit::graphic::Shader::operator=(Shader&& other) noexcept -> Shader& {
    if (this != &other) {
        glDeleteProgram(m_RendererID);
        m_RendererID = other.m_RendererID;
        m_FilePath = std::move(other.m_FilePath);
        m_UniformLocationCach = std::move(other.m_UniformLocationCach);
        other.m_RendererID = 0;
    }
    return *this;
}

auto gkit::graphic::Shader::create_shader(const std::string& vertexShader, const std::string& fragmentShader) -> uint32_t {
    uint32_t program = glCreateProgram();
    uint32_t vs = compile_shader(GL_VERTEX_SHADER, vertexShader);
    uint32_t fs = compile_shader(GL_FRAGMENT_SHADER, fragmentShader);

    if (vs == 0 || fs == 0) {
        glDeleteProgram(program);
        return 0;
    }

    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);

    int linkResult;
    glGetProgramiv(program, GL_LINK_STATUS, &linkResult);
    if (linkResult == GL_FALSE) {
        int length;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
        char* message = (char*)alloca(length * sizeof(char));
        glGetProgramInfoLog(program, length, &length, message);
        gkit::utils::Log::Message msg;
        msg.level = gkit::utils::Log::LogLevel::Error;
        msg.functions = static_cast<std::uint8_t>(gkit::utils::Log::LogFunction::Both);
        msg.message = "Failed to link shader program: " + std::string(message);
        gkit::utils::Log::instance().log(std::move(msg));
        glDeleteProgram(program);
        glDeleteShader(vs);
        glDeleteShader(fs);
        return 0;
    }

    glDetachShader(program, vs);
    glDetachShader(program, fs);
    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}

auto gkit::graphic::Shader::parse_shader(const std::string& filePath) -> ShaderProgramSource {
    gkit::utils::Log::Message pathMsg;
    pathMsg.level = gkit::utils::Log::LogLevel::Info;
    pathMsg.functions = static_cast<std::uint8_t>(gkit::utils::Log::LogFunction::Both);
    pathMsg.message = "Parsing shader: " + filePath;
    gkit::utils::Log::instance().log(std::move(pathMsg));
    std::ifstream stream(filePath);

    /**
     * @brief shader enum type
     */
    enum class ShaderType {
        NONE = -1,
        VERTEX = 0,
        FRAGMENT = 1
    };

    std::string line;
    std::stringstream ss[2];
    ShaderType type = ShaderType::NONE;
    while (getline(stream, line)) {
        if (line.find("#shader") != std::string::npos) {
            if (line.find("vertex") != std::string::npos)
                type = ShaderType::VERTEX;
            else if (line.find("fragment") != std::string::npos)
                type = ShaderType::FRAGMENT;
        } else if (type != ShaderType::NONE) {
            ss[(int)type] << line << '\n';
        }
    }

    if (ss[0].str().empty()) {
        gkit::utils::Log::Message msg;
        msg.level = gkit::utils::Log::LogLevel::Error;
        msg.functions = static_cast<std::uint8_t>(gkit::utils::Log::LogFunction::Both);
        msg.message = "No vertex shader found in: " + filePath;
        gkit::utils::Log::instance().log(std::move(msg));
    }
    if (ss[1].str().empty()) {
        gkit::utils::Log::Message msg;
        msg.level = gkit::utils::Log::LogLevel::Error;
        msg.functions = static_cast<std::uint8_t>(gkit::utils::Log::LogFunction::Both);
        msg.message = "No fragment shader found in: " + filePath;
        gkit::utils::Log::instance().log(std::move(msg));
    }

    gkit::utils::Log::Message vsMsg;
    vsMsg.level = gkit::utils::Log::LogLevel::Info;
    vsMsg.functions = static_cast<std::uint8_t>(gkit::utils::Log::LogFunction::Both);
    vsMsg.message = ss[0].str();
    gkit::utils::Log::instance().log(std::move(vsMsg));

    gkit::utils::Log::Message fsMsg;
    fsMsg.level = gkit::utils::Log::LogLevel::Info;
    fsMsg.functions = static_cast<std::uint8_t>(gkit::utils::Log::LogFunction::Both);
    fsMsg.message = ss[1].str();
    gkit::utils::Log::instance().log(std::move(fsMsg));

    return { .vertexShader=ss[0].str(), .fragmentShader=ss[1].str() };
}

auto gkit::graphic::Shader::compile_shader(uint32_t type, const std::string& source) -> uint32_t {
    uint32_t id = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);

    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE) {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        char* message = (char*)alloca(length * sizeof(char));
        glGetShaderInfoLog(id, length, &length, message);
        gkit::utils::Log::Message msg;
        msg.level = gkit::utils::Log::LogLevel::Error;
        msg.functions = static_cast<std::uint8_t>(gkit::utils::Log::LogFunction::Both);
        msg.message = "Failed to compile " + std::string(type == GL_VERTEX_SHADER ? "vertex" : "fragment") + " shader: " + message;
        gkit::utils::Log::instance().log(std::move(msg));
        glDeleteShader(id);
        return 0;
    }

    return id;
}

auto gkit::graphic::Shader::bind() const -> void {
    glUseProgram(m_RendererID);
}
auto gkit::graphic::Shader::unbind() const -> void {
    glUseProgram(0);
}

auto gkit::graphic::Shader::set_uniform_1i(const std::string& name, int value) -> void {
    glUniform1i(get_uniform_location(name), value);
}

auto gkit::graphic::Shader::set_uniform_1f(const std::string& name, float value) -> void {
    glUniform1f(get_uniform_location(name), value);
}

auto gkit::graphic::Shader::set_uniform_4f(const std::string& name, float v0, float v1, float v2, float v3) -> void {
    glUniform4f(get_uniform_location(name), v0, v1, v2, v3);
}

auto gkit::graphic::Shader::set_uniform_mat_4f(const std::string& name, const float* matrix) -> void {
    glUniformMatrix4fv(get_uniform_location(name), 1, GL_FALSE, matrix);
}

auto gkit::graphic::Shader::set_uniform_mat_3f(const std::string& name, const float* matrix) -> void {
    glUniformMatrix3fv(get_uniform_location(name), 1, GL_FALSE, matrix);
}

auto gkit::graphic::Shader::set_uniform_vec_4f(const std::string& name, const float* vector4) -> void {
    glUniform4fv(get_uniform_location(name), 1, vector4);
}

auto gkit::graphic::Shader::set_uniform_vec_3f(const std::string& name, const float* vector3) -> void {
    glUniform3fv(get_uniform_location(name), 1, vector3);
}

auto gkit::graphic::Shader::set_uniform_1iv(const std::string& name, const int sz, const int* ind) -> void {
    glUniform1iv(get_uniform_location(name), sz, ind);
}

auto gkit::graphic::Shader::get_uniform_location(const std::string& name) -> int {
    if (m_UniformLocationCach.find(name) != m_UniformLocationCach.end())
        return m_UniformLocationCach[name];
    int location = glGetUniformLocation(m_RendererID, name.c_str());
    if (location == -1) {
        gkit::utils::Log::Message msg;
        msg.level = gkit::utils::Log::LogLevel::Warning;
        msg.functions = static_cast<std::uint8_t>(gkit::utils::Log::LogFunction::Both);
        msg.message = "Warning: uniform '" + name + "' doesn't exist!";
        gkit::utils::Log::instance().log(std::move(msg));
    }
    m_UniformLocationCach[name] = location;
    return location;
}