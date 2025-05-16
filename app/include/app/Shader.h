//
// Created by user on 5/16/25.
//

#ifndef LEARNOPENGL_APP_SHADER_H
#define LEARNOPENGL_APP_SHADER_H

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <optional>
#include <print>
#include <string_view>

namespace lgl
{
    class Shader
    {
    public:
        [[nodiscard]] GLuint program() const;

        [[nodiscard]] GLint getUniformLocation(std::string_view name) const;

        template<typename... Args>
        void setUniform(const std::string_view name, Args... args)
        {
            setUniform(getUniformLocation(name), std::forward<Args>(args)...);
        }

    private:
        GLuint m_program;

        static void setUniform(GLint location, GLint value);
        static void setUniform(GLint location, GLfloat value);
        static void setUniform(GLint location, GLuint value);
        static void setUniform(GLint location, bool value);
        static void setUniform(GLint location, const glm::ivec1& value);
        static void setUniform(GLint location, const glm::fvec1& value);
        static void setUniform(GLint location, const glm::uvec1& value);
        static void setUniform(GLint location, const glm::bvec1& value);

        static void setUniform(GLint location, GLint x, GLint y);
        static void setUniform(GLint location, GLfloat x, GLfloat y);
        static void setUniform(GLint location, GLuint x, GLuint y);
        static void setUniform(GLint location, bool x, bool y);
        static void setUniform(GLint location, const glm::ivec2& value);
        static void setUniform(GLint location, const glm::fvec2& value);
        static void setUniform(GLint location, const glm::uvec2& value);
        static void setUniform(GLint location, const glm::bvec2& value);

        static void setUniform(GLint location, GLint x, GLint y, GLint z);
        static void setUniform(GLint location, GLfloat x, GLfloat y, GLfloat z);
        static void setUniform(GLint location, GLuint x, GLuint y, GLuint z);
        static void setUniform(GLint location, bool x, bool y, bool z);
        static void setUniform(GLint location, const glm::ivec3& value);
        static void setUniform(GLint location, const glm::fvec3& value);
        static void setUniform(GLint location, const glm::uvec3& value);
        static void setUniform(GLint location, const glm::bvec3& value);

        static void setUniform(GLint location, GLint x, GLint y, GLint z, GLint w);
        static void setUniform(GLint location, GLfloat x, GLfloat y, GLfloat z, GLfloat w);
        static void setUniform(GLint location, GLuint x, GLuint y, GLuint z, GLuint w);
        static void setUniform(GLint location, bool x, bool y, bool z, bool w);
        static void setUniform(GLint location, const glm::ivec4& value);
        static void setUniform(GLint location, const glm::fvec4& value);
        static void setUniform(GLint location, const glm::uvec4& value);
        static void setUniform(GLint location, const glm::bvec4& value);
    };
} // lgl

#endif //LEARNOPENGL_APP_SHADER_H
