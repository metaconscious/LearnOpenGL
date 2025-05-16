//
// Created by user on 5/16/25.
//

#include "app/Shader.h"

#include <glm/gtc/type_ptr.hpp>

namespace lgl
{
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
        glUniform1iv(location, 1, glm::value_ptr(value));
    }

    void Shader::setUniform(const GLint location, const glm::fvec1& value)
    {
        glUniform1fv(location, 1, glm::value_ptr(value));
    }

    void Shader::setUniform(const GLint location, const glm::uvec1& value)
    {
        glUniform1uiv(location, 1, glm::value_ptr(value));
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
