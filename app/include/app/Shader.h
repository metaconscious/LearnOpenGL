//
// Created by user on 5/27/25.
//

#ifndef LEARNOPENGL_APP_SHADER_H
#define LEARNOPENGL_APP_SHADER_H

#include <filesystem>
#include <string_view>
#include "glad/glad.h"

namespace lgl
{
    class Shader
    {
    public:
        enum class Type
        {
            Vertex,
            Fragment
        };

        static Shader loadFromFile(const std::filesystem::path& shaderPath);

        static Shader loadFromFile(const std::filesystem::path& shaderPath, Type shaderType);

        Shader(const Shader& other) = delete;
        Shader(Shader&& other) noexcept;
        Shader& operator=(const Shader& other) = delete;
        Shader& operator=(Shader&& other) noexcept;

        ~Shader();

        [[nodiscard]] GLuint getId() const;

    private:
        GLuint m_shaderId;

        explicit Shader(Type shaderType);

        void compileAndValidate(std::string_view source) const;

        void compile(std::string_view source) const;

        void validate() const;

        static Shader loadFromFile(const std::filesystem::path& shaderPath,
                                   Type shaderType,
                                   bool bypassValidation);

        static void checkShaderCompilingSuccessfulness(GLuint shaderId);

        static void checkShaderCompilingSuccessfulness(const Shader& shader);
    };

    constexpr auto gl_cast(const Shader::Type shaderType)
    {
        switch (shaderType)
        {
            case Shader::Type::Vertex:
                return GL_VERTEX_SHADER;
            case Shader::Type::Fragment:
                return GL_FRAGMENT_SHADER;
            default:
                throw std::runtime_error("Invalid/Unsupported shader type");
        }
    }
} // lgl

#endif //LEARNOPENGL_APP_SHADER_H
