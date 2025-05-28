//
// Created by user on 5/27/25.
//

#include "app/Shader.h"

#include <print>
#include "app/utilities.h"

namespace lgl
{
    Shader Shader::loadFromFile(const std::filesystem::path& shaderPath)
    {
        if (!std::filesystem::exists(shaderPath))
        {
            throw std::runtime_error("Shader file does not exist");
        }
        if (!is_regular_file(shaderPath))
        {
            throw std::runtime_error("Shader file is not a regular file");
        }
        if (shaderPath.has_extension())
        {
            const auto extension{ shaderPath.extension().string() };
            if (extension == ".vert")
            {
                return loadFromFile(shaderPath, Type::Vertex, true);
            }
            if (extension == ".frag")
            {
                return loadFromFile(shaderPath, Type::Fragment, true);
            }
            throw std::runtime_error("Invalid/Unsupported shader file extension");
        }
        throw std::runtime_error("Unrecognized shader type");
    }

    Shader Shader::loadFromFile(const std::filesystem::path& shaderPath, const Type shaderType)
    {
        return loadFromFile(shaderPath, shaderType, false);
    }

    Shader::Shader(Shader&& other) noexcept = default;

    Shader& Shader::operator=(Shader&& other) noexcept = default;

    Shader::~Shader()
    {
        glDeleteShader(m_shaderId);
    }

    GLuint Shader::getId() const
    {
        return m_shaderId;
    }

    Shader::Shader(const Type shaderType)
        : m_shaderId{ glCreateShader(gl_cast(shaderType)) }
    {
    }

    void Shader::compileAndValidate(const std::string_view source) const
    {
        compile(source);
        validate();
    }

    void Shader::compile(const std::string_view source) const
    {
        const auto rawSource{ source.data() };
        glShaderSource(m_shaderId, 1, &rawSource, nullptr);
        glCompileShader(m_shaderId);
    }

    void Shader::validate() const
    {
        checkShaderCompilingSuccessfulness(*this);
    }

    Shader Shader::loadFromFile(const std::filesystem::path& shaderPath,
                                const Type shaderType,
                                const bool bypassValidation)
    {
        if (!bypassValidation)
        {
            if (!std::filesystem::exists(shaderPath))
            {
                throw std::runtime_error("Shader file does not exist");
            }
            if (!is_regular_file(shaderPath))
            {
                throw std::runtime_error("Shader file is not a regular file");
            }
        }
        Shader shader{ shaderType };
        shader.compileAndValidate(readAll(shaderPath));
        return shader;
    }

    void Shader::checkShaderCompilingSuccessfulness(const GLuint shaderId)
    {
        GLint success{};
        glGetShaderiv(shaderId, GL_COMPILE_STATUS, &success);
        if (success == GL_FALSE)
        {
            GLchar infoLog[512];
            glGetShaderInfoLog(shaderId, 512, nullptr, infoLog);

            GLint shaderType{};
            glGetShaderiv(shaderId, GL_SHADER_TYPE, &shaderType);
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

    void Shader::checkShaderCompilingSuccessfulness(const Shader& shader)
    {
        checkShaderCompilingSuccessfulness(shader.m_shaderId);
    }
} // csv
