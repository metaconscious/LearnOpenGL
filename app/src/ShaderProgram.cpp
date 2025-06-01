//
// Created by user on 5/16/25.
//

#include "app/ShaderProgram.h"

#include <fstream>
#include <print>
#include <glm/gtc/type_ptr.hpp>
#include "app/utilities.h"


namespace lgl
{
    ShaderProgram ShaderProgram::load(const std::filesystem::path& vertexShaderFile,
                                      const std::filesystem::path& fragmentShaderFile)
    {
        const auto vertexShader{ Shader::loadFromFile(vertexShaderFile, Shader::Type::Vertex) };

        const auto fragmentShader{ Shader::loadFromFile(fragmentShaderFile, Shader::Type::Fragment) };

        ShaderProgram shaderProgram{};
        shaderProgram.attachShader(vertexShader);
        shaderProgram.attachShader(fragmentShader);
        shaderProgram.linkAndValidate();

        return shaderProgram;
    }

    ShaderProgram::ShaderProgram(const GLuint program)
        : m_programId{ program }
    {
    }

    ShaderProgram::ShaderProgram()
        : ShaderProgram{ glCreateProgram() }
    {
    }

    ShaderProgram::ShaderProgram(ShaderProgram&& other) noexcept
    = default;

    ShaderProgram& ShaderProgram::operator=(ShaderProgram&& other) noexcept
    = default;

    ShaderProgram::~ShaderProgram()
    {
        glDeleteProgram(m_programId);
    }

    GLuint ShaderProgram::getId() const
    {
        return m_programId;
    }

    GLint ShaderProgram::getUniformLocation(const std::string_view name) const
    {
        return glGetUniformLocation(m_programId, name.data());
    }

    void ShaderProgram::use() const
    {
        glUseProgram(m_programId);
    }

    void ShaderProgram::setUniform(const GLint location, const GLint value)
    {
        glUniform1i(location, value);
    }

    void ShaderProgram::setUniform(const GLint location, const GLfloat value)
    {
        glUniform1f(location, value);
    }

    void ShaderProgram::setUniform(const GLint location, const GLuint value)
    {
        glUniform1ui(location, value);
    }

    void ShaderProgram::setUniform(const GLint location, const bool value)
    {
        setUniform(location, static_cast<GLint>(value));
    }

    void ShaderProgram::setUniform(const GLint location, const glm::ivec1& value)
    {
        glUniform1iv(location, 1, &value[0]);
    }

    void ShaderProgram::setUniform(const GLint location, const glm::fvec1& value)
    {
        glUniform1fv(location, 1, &value[0]);
    }

    void ShaderProgram::setUniform(const GLint location, const glm::uvec1& value)
    {
        glUniform1uiv(location, 1, &value[0]);
    }

    void ShaderProgram::setUniform(const GLint location, const glm::bvec1& value)
    {
        setUniform(location, glm::ivec1{ value });
    }

    void ShaderProgram::setUniform(const GLint location, const glm::dvec1& value)
    {
        std::println(stderr, "Conversion from double to float may result in loss of precision");
        setUniform(location, glm::fvec1{ value });
    }

    void ShaderProgram::setUniform(const GLint location, const GLint x, const GLint y)
    {
        glUniform2i(location, x, y);
    }

    void ShaderProgram::setUniform(const GLint location, const GLfloat x, const GLfloat y)
    {
        glUniform2f(location, x, y);
    }

    void ShaderProgram::setUniform(const GLint location, const GLuint x, const GLuint y)
    {
        glUniform2ui(location, x, y);
    }

    void ShaderProgram::setUniform(const GLint location, const bool x, const bool y)
    {
        setUniform(location, static_cast<GLint>(x), static_cast<GLint>(y));
    }

    void ShaderProgram::setUniform(const GLint location, const glm::ivec2& value)
    {
        glUniform2iv(location, 1, glm::value_ptr(value));
    }

    void ShaderProgram::setUniform(const GLint location, const glm::fvec2& value)
    {
        glUniform2fv(location, 1, glm::value_ptr(value));
    }

    void ShaderProgram::setUniform(const GLint location, const glm::uvec2& value)
    {
        glUniform2uiv(location, 1, glm::value_ptr(value));
    }

    void ShaderProgram::setUniform(const GLint location, const glm::bvec2& value)
    {
        setUniform(location, glm::ivec2{ value });
    }

    void ShaderProgram::setUniform(const GLint location, const glm::dvec2& value)
    {
        std::println(stderr, "Conversion from double to float may result in loss of precision");
        setUniform(location, glm::fvec2{ value });
    }

    void ShaderProgram::setUniform(const GLint location, const glm::fmat2x2& value)
    {
        glUniformMatrix2fv(location, 1, GL_FALSE, glm::value_ptr(value));
    }

    void ShaderProgram::setUniform(const GLint location, const GLint x, const GLint y, const GLint z)
    {
        glUniform3i(location, x, y, z);
    }

    void ShaderProgram::setUniform(const GLint location, const GLfloat x, const GLfloat y, const GLfloat z)
    {
        glUniform3f(location, x, y, z);
    }

    void ShaderProgram::setUniform(const GLint location, const GLuint x, const GLuint y, const GLuint z)
    {
        glUniform3ui(location, x, y, z);
    }

    void ShaderProgram::setUniform(const GLint location, const bool x, const bool y, const bool z)
    {
        setUniform(location, static_cast<GLint>(x), static_cast<GLint>(y), static_cast<GLint>(z));
    }

    void ShaderProgram::setUniform(const GLint location, const glm::ivec3& value)
    {
        glUniform3iv(location, 1, glm::value_ptr(value));
    }

    void ShaderProgram::setUniform(const GLint location, const glm::fvec3& value)
    {
        glUniform3fv(location, 1, glm::value_ptr(value));
    }

    void ShaderProgram::setUniform(const GLint location, const glm::uvec3& value)
    {
        glUniform3uiv(location, 1, glm::value_ptr(value));
    }

    void ShaderProgram::setUniform(const GLint location, const glm::bvec3& value)
    {
        setUniform(location, glm::ivec3{ value });
    }

    void ShaderProgram::setUniform(const GLint location, const glm::dvec3& value)
    {
        std::println(stderr, "Conversion from double to float may result in loss of precision");
        setUniform(location, glm::fvec3{ value });
    }

    void ShaderProgram::setUniform(const GLint location, const glm::fmat3x2& value)
    {
        glUniformMatrix2x3fv(location, 1, GL_FALSE, glm::value_ptr(value));
    }

    void ShaderProgram::setUniform(const GLint location, const glm::fmat3x3& value)
    {
        glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(value));
    }

    void ShaderProgram::setUniform(const GLint location, const GLint x, const GLint y, const GLint z, const GLint w)
    {
        glUniform4i(location, x, y, z, w);
    }

    void ShaderProgram::setUniform(const GLint location, const GLfloat x, const GLfloat y, const GLfloat z,
                                   const GLfloat w)
    {
        glUniform4f(location, x, y, z, w);
    }

    void ShaderProgram::setUniform(const GLint location, const GLuint x, const GLuint y, const GLuint z, const GLuint w)
    {
        glUniform4ui(location, x, y, z, w);
    }

    void ShaderProgram::setUniform(const GLint location, const bool x, const bool y, const bool z, const bool w)
    {
        setUniform(location,
                   static_cast<GLint>(x), static_cast<GLint>(y), static_cast<GLint>(z), static_cast<GLint>(w));
    }

    void ShaderProgram::setUniform(const GLint location, const glm::ivec4& value)
    {
        glUniform4iv(location, 1, glm::value_ptr(value));
    }

    void ShaderProgram::setUniform(const GLint location, const glm::fvec4& value)
    {
        glUniform4fv(location, 1, glm::value_ptr(value));
    }

    void ShaderProgram::setUniform(const GLint location, const glm::uvec4& value)
    {
        glUniform4uiv(location, 1, glm::value_ptr(value));
    }

    void ShaderProgram::setUniform(const GLint location, const glm::bvec4& value)
    {
        setUniform(location, glm::ivec4{ value });
    }

    void ShaderProgram::setUniform(const GLint location, const glm::dvec4& value)
    {
        std::println(stderr, "Conversion from double to float may result in loss of precision");
        setUniform(location, glm::fvec4{ value });
    }

    void ShaderProgram::setUniform(const GLint location, const glm::fmat2x4& value)
    {
        glUniformMatrix2x4fv(location, 1, GL_FALSE, glm::value_ptr(value));
    }

    void ShaderProgram::setUniform(const GLint location, const glm::fmat3x4& value)
    {
        glUniformMatrix3x4fv(location, 1, GL_FALSE, glm::value_ptr(value));
    }

    void ShaderProgram::setUniform(const GLint location, const glm::fmat4x4& value)
    {
        glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(value));
    }

    void ShaderProgram::checkProgramLinkageSuccessfulness(const ShaderProgram& shaderProgram)
    {
        checkProgramLinkageSuccessfulness(shaderProgram.m_programId);
    }

    void ShaderProgram::checkProgramLinkageSuccessfulness(const GLuint shaderProgram)
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

    void ShaderProgram::validate() const
    {
        checkProgramLinkageSuccessfulness(*this);
    }

    void ShaderProgram::attachShader(const Shader& shader) const
    {
        glAttachShader(m_programId, shader.getId());
    }

    void ShaderProgram::link() const
    {
        glLinkProgram(m_programId);
    }

    void ShaderProgram::linkAndValidate() const
    {
        link();
        validate();
    }
} // lgl
