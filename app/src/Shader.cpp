//
// Created by user on 5/16/25.
//

#include "app/Shader.h"

#include <glm/gtc/type_ptr.hpp>

#include <fstream>
#include <print>

[[nodiscard]] std::string readAll(const std::filesystem::path& filepath)
{
    std::ifstream file{ filepath, std::ios::binary };
    if (!file.is_open())
    {
        std::println(stderr, "Failed to open file at '{}'", filepath.string());
        throw std::runtime_error("Could not open file");
    }

    const auto fileSize{ std::filesystem::file_size(filepath) };
    if (fileSize == 0)
    {
        std::println(stderr, "Reading an empty file: {}", filepath.string());
        return {};
    }

    std::string content(fileSize, '\0');
    file.read(content.data(), static_cast<std::streamsize>(content.size()));
    return content;
}

void checkShaderCompilingSuccessfulness(const GLuint shader)
{
    GLint success{};
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (success == GL_FALSE)
    {
        GLchar infoLog[512];
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);

        GLint shaderType{};
        glGetShaderiv(shader, GL_SHADER_TYPE, &shaderType);
        std::string shaderTypeName{};
        switch (shaderType)
        {
            case GL_VERTEX_SHADER:
                shaderTypeName = "vertex";
                break;
            case GL_FRAGMENT_SHADER:
                shaderTypeName = "fragment";
                break;
            default:
                throw std::runtime_error("Not implement yet");
        }

        std::println(stderr, "{} shader compilation failed:\n{}", shaderTypeName, infoLog);
        throw std::runtime_error("Shader compilation failed");
    }
}

void checkProgramLinkageSuccessfulness(const GLuint shaderProgram)
{
    GLint success{};
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (success == GL_FALSE)
    {
        GLchar infoLog[512];
        glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
        std::println(stderr, "Shader program linkage failed:\n{}", infoLog);
        throw std::runtime_error("Shader program linkage failed");
    }
}

namespace lgl
{
    Shader Shader::load(const std::filesystem::path& vertexShaderFile,
                              const std::filesystem::path& fragmentShaderFile)
    {
        const auto vertexShaderSource{ readAll(vertexShaderFile) };
        const auto* vertexShaderSourceRaw{ vertexShaderSource.c_str() };
        const auto vertexShader{ glCreateShader(GL_VERTEX_SHADER) };
        glShaderSource(vertexShader, 1, &vertexShaderSourceRaw, nullptr);
        glCompileShader(vertexShader);

        checkShaderCompilingSuccessfulness(vertexShader);

        const auto fragmentShaderSource{ readAll(fragmentShaderFile) };
        const auto* fragmentShaderSourceRaw{ fragmentShaderSource.c_str() };
        const auto fragmentShader{ glCreateShader(GL_FRAGMENT_SHADER) };
        glShaderSource(fragmentShader, 1, &fragmentShaderSourceRaw, nullptr);
        glCompileShader(fragmentShader);

        checkShaderCompilingSuccessfulness(fragmentShader);

        const auto shaderProgram{ glCreateProgram() };
        glAttachShader(shaderProgram, vertexShader);
        glAttachShader(shaderProgram, fragmentShader);
        glLinkProgram(shaderProgram);

        checkProgramLinkageSuccessfulness(shaderProgram);

        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);

        return Shader{ shaderProgram };
    }

    Shader::Shader(const GLuint program)
        : m_program{ program }
    {
    }

    GLuint Shader::program() const
    {
        return m_program;
    }

    GLint Shader::getUniformLocation(const std::string_view name) const
    {
        const auto location{ glGetUniformLocation(program(), name.data()) };
        if (location == -1)
        {
            std::println(stderr, "Uniform location '{}' not found", name);
            throw std::runtime_error("Uniform location not found");
        }
        return location;
    }

    void Shader::use() const
    {
        glUseProgram(m_program);
    }

    void Shader::setUniform(const GLint location, const GLint value)
    {
        glUniform1i(location, value);
    }

    void Shader::setUniform(const GLint location, const GLfloat value)
    {
        glUniform1f(location, value);
    }

    void Shader::setUniform(const GLint location, const GLuint value)
    {
        glUniform1ui(location, value);
    }

    void Shader::setUniform(const GLint location, const bool value)
    {
        setUniform(location, static_cast<GLint>(value));
    }

    void Shader::setUniform(const GLint location, const glm::ivec1& value)
    {
        glUniform1iv(location, 1, &value[0]);
    }

    void Shader::setUniform(const GLint location, const glm::fvec1& value)
    {
        glUniform1fv(location, 1, &value[0]);
    }

    void Shader::setUniform(const GLint location, const glm::uvec1& value)
    {
        glUniform1uiv(location, 1, &value[0]);
    }

    void Shader::setUniform(const GLint location, const glm::bvec1& value)
    {
        setUniform(location, glm::ivec1{ value });
    }

    void Shader::setUniform(const GLint location, const GLint x, const GLint y)
    {
        glUniform2i(location, x, y);
    }

    void Shader::setUniform(const GLint location, const GLfloat x, const GLfloat y)
    {
        glUniform2f(location, x, y);
    }

    void Shader::setUniform(const GLint location, const GLuint x, const GLuint y)
    {
        glUniform2ui(location, x, y);
    }

    void Shader::setUniform(const GLint location, const bool x, const bool y)
    {
        setUniform(location, static_cast<GLint>(x), static_cast<GLint>(y));
    }

    void Shader::setUniform(const GLint location, const glm::ivec2& value)
    {
        glUniform2iv(location, 2, glm::value_ptr(value));
    }

    void Shader::setUniform(const GLint location, const glm::fvec2& value)
    {
        glUniform2fv(location, 2, glm::value_ptr(value));
    }

    void Shader::setUniform(const GLint location, const glm::uvec2& value)
    {
        glUniform2uiv(location, 2, glm::value_ptr(value));
    }

    void Shader::setUniform(const GLint location, const glm::bvec2& value)
    {
        setUniform(location, glm::ivec2{ value });
    }

    void Shader::setUniform(const GLint location, const GLint x, const GLint y, const GLint z)
    {
        glUniform3i(location, x, y, z);
    }

    void Shader::setUniform(const GLint location, const GLfloat x, const GLfloat y, const GLfloat z)
    {
        glUniform3f(location, x, y, z);
    }

    void Shader::setUniform(const GLint location, const GLuint x, const GLuint y, const GLuint z)
    {
        glUniform3ui(location, x, y, z);
    }

    void Shader::setUniform(const GLint location, const bool x, const bool y, const bool z)
    {
        setUniform(location, static_cast<GLint>(x), static_cast<GLint>(y), static_cast<GLint>(z));
    }

    void Shader::setUniform(const GLint location, const glm::ivec3& value)
    {
        glUniform3iv(location, 3, glm::value_ptr(value));
    }

    void Shader::setUniform(const GLint location, const glm::fvec3& value)
    {
        glUniform3fv(location, 3, glm::value_ptr(value));
    }

    void Shader::setUniform(const GLint location, const glm::uvec3& value)
    {
        glUniform3uiv(location, 3, glm::value_ptr(value));
    }

    void Shader::setUniform(const GLint location, const glm::bvec3& value)
    {
        setUniform(location, glm::ivec3{ value });
    }

    void Shader::setUniform(const GLint location, const GLint x, const GLint y, const GLint z, const GLint w)
    {
        glUniform4i(location, x, y, z, w);
    }

    void Shader::setUniform(const GLint location, const GLfloat x, const GLfloat y, const GLfloat z, const GLfloat w)
    {
        glUniform4f(location, x, y, z, w);
    }

    void Shader::setUniform(const GLint location, const GLuint x, const GLuint y, const GLuint z, const GLuint w)
    {
        glUniform4ui(location, x, y, z, w);
    }

    void Shader::setUniform(const GLint location, const bool x, const bool y, const bool z, const bool w)
    {
        setUniform(location,
                   static_cast<GLint>(x), static_cast<GLint>(y), static_cast<GLint>(z), static_cast<GLint>(w));
    }

    void Shader::setUniform(const GLint location, const glm::ivec4& value)
    {
        glUniform4iv(location, 4, glm::value_ptr(value));
    }

    void Shader::setUniform(const GLint location, const glm::fvec4& value)
    {
        glUniform4fv(location, 4, glm::value_ptr(value));
    }

    void Shader::setUniform(const GLint location, const glm::uvec4& value)
    {
        glUniform4uiv(location, 4, glm::value_ptr(value));
    }

    void Shader::setUniform(const GLint location, const glm::bvec4& value)
    {
        setUniform(location, glm::ivec4{ value });
    }
} // lgl
