#include "gkit/graphic/Shader.hpp"

#include <glad/gl.h>

#include <sstream>
#include <fstream>
#include <iostream>

gkit::graphic::Shader::Shader(const std::string& filepath)
	: m_FilePath(filepath), m_RendererID(0)
{
    ShaderProgramSource source = ParseShader(filepath);
    m_RendererID = CreateShader(source.vertexShader, source.fragmenShader);
}

gkit::graphic::Shader::~Shader()
{
    glDeleteProgram(m_RendererID);
}

auto gkit::graphic::Shader::CreateShader(const std::string& vertexShader, const std::string& fragmentShader) -> uint32_t
{
    uint32_t program = glCreateProgram();
    uint32_t vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
    uint32_t fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);

    glValidateProgram(program);

    return program;
}

auto gkit::graphic::Shader::ParseShader(const std::string& filePath) -> ShaderProgramSource
{
    std::cout << filePath << std::endl;
    std::ifstream stream(filePath);

    //shader enum type
    enum class ShaderType
    {
        NONE = -1,
        VERTEX = 0,
        FRAGMENT = 1
    };

    std::string line;
    std::stringstream ss[2];//shader string stream
    ShaderType type = ShaderType::NONE;
    while (getline(stream, line))
    {
        if (line.find("#shader") != std::string::npos)
        {
            if (line.find("vertex") != std::string::npos)
                type = ShaderType::VERTEX;
            else if (line.find("fragment") != std::string::npos)
                type = ShaderType::FRAGMENT;
        }
        else
        {
            ss[(int)type] << line << '\n';
        }
    }

    std::cout << ss[0].str() << std::endl;
    std::cout << ss[1].str() << std::endl;

    return { ss[0].str(), ss[1].str() };
}

auto gkit::graphic::Shader::CompileShader(uint32_t type, const std::string& source) -> uint32_t
{
    uint32_t id = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);

    //Get Error Info
    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE)
    {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        char* message = (char*)alloca(length * sizeof(char));
        glGetShaderInfoLog(id, length, &length, message);
        std::cout << "==================================================" << std::endl;
        std::cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << std::endl;
        std::cout << "==================================================" << std::endl;
        std::cout << message << std::endl;
        glDeleteShader(id);
        return 0;
    }

    return id;
}

auto gkit::graphic::Shader::Bind() const -> void
{
    glUseProgram(m_RendererID);
}
auto gkit::graphic::Shader::Unbind() const -> void
{
    glUseProgram(0);
}

//set uniforms

auto gkit::graphic::Shader::SetUniform1i(const std::string& name, int value) -> void
{
    glUniform1i(GetUniformLocation(name), value);
}

auto gkit::graphic::Shader::SetUniform1f(const std::string& name, float value) -> void
{
    glUniform1f(GetUniformLocation(name), value);
}

auto gkit::graphic::Shader::SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3) -> void
{
    glUniform4f(GetUniformLocation(name), v0, v1, v2, v3);
}

auto gkit::graphic::Shader::SetUniformMat4f(const std::string& name, const float* matrix) -> void
{
    glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, matrix);
}

auto gkit::graphic::Shader::SetUniformMat3f(const std::string& name, const float* matrix) -> void
{
    glUniformMatrix3fv(GetUniformLocation(name), 1, GL_FALSE, matrix);
}

auto gkit::graphic::Shader::SetUniformVec4f(const std::string& name, const float* vector4) -> void
{
    glUniform4fv(GetUniformLocation(name), 1, vector4);
}

auto gkit::graphic::Shader::SetUniformVec3f(const std::string& name, const float* vector3) -> void
{
    glUniform3fv(GetUniformLocation(name), 1, vector3);
}


auto gkit::graphic::Shader::SetUniform1iv(const std::string& name, const int sz, const int* ind) -> void
{
    glUniform1iv(GetUniformLocation(name), sz, ind);
}


auto gkit::graphic::Shader::GetUniformLocation(const std::string& name) -> int
{
    if (m_UniformLocationCach.find(name) != m_UniformLocationCach.end())
        return m_UniformLocationCach[name];
    int location = glGetUniformLocation(m_RendererID, name.c_str());
    if (location == -1)
    {
        std::cout << "==================================================" << std::endl;
        std::cout << "Warning: uniform '" << name << "' doesn't exist!" << std::endl;
        std::cout << "==================================================" << std::endl;
    }
    m_UniformLocationCach[name] = location;
    return location;
}