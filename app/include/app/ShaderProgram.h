//
// Created by user on 5/16/25.
//

#ifndef LEARNOPENGL_APP_SHADERPROGRAM_H
#define LEARNOPENGL_APP_SHADERPROGRAM_H

#include <filesystem>
#include <print>
#include <string_view>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include "app/Shader.h"

namespace lgl
{
    class Shader;

    class ShaderProgram
    {
    public:
        [[nodiscard]] static ShaderProgram load(const std::filesystem::path& vertexShaderFile,
                                                const std::filesystem::path& fragmentShaderFile);

        explicit ShaderProgram(GLuint program);
        ShaderProgram();

        ShaderProgram(const ShaderProgram& other) = delete;
        ShaderProgram(ShaderProgram&& other) noexcept;
        ShaderProgram& operator=(const ShaderProgram& other) = delete;
        ShaderProgram& operator=(ShaderProgram&& other) noexcept;

        ~ShaderProgram();

        [[nodiscard]] GLuint getId() const;

        [[nodiscard]] GLint getUniformLocation(std::string_view name) const;

        template<typename... Args>
        void setUniform(std::string_view name, Args... args) const;

        void use() const;

    private:
        GLuint m_programId;

        static void setUniform(GLint location, GLint value);
        static void setUniform(GLint location, GLfloat value);
        static void setUniform(GLint location, GLuint value);
        static void setUniform(GLint location, bool value);
        static void setUniform(GLint location, const glm::ivec1& value);
        static void setUniform(GLint location, const glm::fvec1& value);
        static void setUniform(GLint location, const glm::uvec1& value);
        static void setUniform(GLint location, const glm::bvec1& value);
        static void setUniform(GLint location, const glm::dvec1& value);

        static void setUniform(GLint location, GLint x, GLint y);
        static void setUniform(GLint location, GLfloat x, GLfloat y);
        static void setUniform(GLint location, GLuint x, GLuint y);
        static void setUniform(GLint location, bool x, bool y);
        static void setUniform(GLint location, const glm::ivec2& value);
        static void setUniform(GLint location, const glm::fvec2& value);
        static void setUniform(GLint location, const glm::uvec2& value);
        static void setUniform(GLint location, const glm::bvec2& value);
        static void setUniform(GLint location, const glm::dvec2& value);
        static void setUniform(GLint location, const glm::fmat2x2& value);

        static void setUniform(GLint location, GLint x, GLint y, GLint z);
        static void setUniform(GLint location, GLfloat x, GLfloat y, GLfloat z);
        static void setUniform(GLint location, GLuint x, GLuint y, GLuint z);
        static void setUniform(GLint location, bool x, bool y, bool z);
        static void setUniform(GLint location, const glm::ivec3& value);
        static void setUniform(GLint location, const glm::fvec3& value);
        static void setUniform(GLint location, const glm::uvec3& value);
        static void setUniform(GLint location, const glm::bvec3& value);
        static void setUniform(GLint location, const glm::dvec3& value);
        static void setUniform(GLint location, const glm::fmat3x2& value);
        static void setUniform(GLint location, const glm::fmat3x3& value);

        static void setUniform(GLint location, GLint x, GLint y, GLint z, GLint w);
        static void setUniform(GLint location, GLfloat x, GLfloat y, GLfloat z, GLfloat w);
        static void setUniform(GLint location, GLuint x, GLuint y, GLuint z, GLuint w);
        static void setUniform(GLint location, bool x, bool y, bool z, bool w);
        static void setUniform(GLint location, const glm::ivec4& value);
        static void setUniform(GLint location, const glm::fvec4& value);
        static void setUniform(GLint location, const glm::uvec4& value);
        static void setUniform(GLint location, const glm::bvec4& value);
        static void setUniform(GLint location, const glm::dvec4& value);
        static void setUniform(GLint location, const glm::fmat2x4& value);
        static void setUniform(GLint location, const glm::fmat3x4& value);
        static void setUniform(GLint location, const glm::fmat4x4& value);

        static void checkProgramLinkageSuccessfulness(const ShaderProgram& shaderProgram);
        static void checkProgramLinkageSuccessfulness(GLuint shaderProgram);

        void validate() const;

        void attachShader(const Shader& shader) const;

        void link() const;

        void linkAndValidate() const;
    };

    template<typename... Args>
    void ShaderProgram::setUniform(const std::string_view name, Args... args) const
    {
        const auto location{ getUniformLocation(name) };
        if (location == -1)
        {
//            std::println("Warning: Uniform '{}' not found (maybe optimized out)", name);
            return;
        }
        setUniform(location, std::forward<Args>(args)...);
    }
} // lgl

#endif //LEARNOPENGL_APP_SHADERPROGRAM_H
