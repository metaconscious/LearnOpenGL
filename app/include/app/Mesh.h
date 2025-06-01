//
// Created by user on 6/1/25.
//

#ifndef LEARNOPENGL_APP_MESH_H
#define LEARNOPENGL_APP_MESH_H

#include <filesystem>
#include <format>
#include <string_view>
#include <vector>
#include <assimp/scene.h>
#include <glad/glad.h>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include "app/Image.h"
#include "app/ShaderProgram.h"

namespace lgl
{
    class Model;

    constexpr auto MAX_BONE_INFLUENCE{ 4 };

    struct Vertex
    {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 textureCoordinates;
        glm::vec3 tangent;
        glm::vec3 bitangent;
        std::array<int, MAX_BONE_INFLUENCE> boneIds;
        std::array<float, MAX_BONE_INFLUENCE> weights;
    };

    struct Texture
    {
        enum class Type
        {
            DIFFUSE = 0,
            SPECULAR,
            NORMAL,
            HEIGHT,
            Size
        };

        GLuint id;
        Type type;
        std::filesystem::path path;
    };

    class Mesh
    {
    public:
        using vertex_type = Vertex;
        using index_type = GLuint;
        using texture_type = Texture;
        using handle_type = GLuint;
        using vertex_container_type = std::vector<vertex_type>;
        using index_container_type = std::vector<index_type>;
        using texture_container_type = std::vector<texture_type>;

        [[nodiscard]] Mesh(vertex_container_type vertices, index_container_type, texture_container_type textures);

        [[nodiscard]] Mesh(const Model& model, const aiMesh* mesh, const aiScene* scene);

        Mesh(const Mesh& other) = delete;

        Mesh(Mesh&& other) noexcept;

        Mesh& operator=(const Mesh& other) = delete;

        Mesh& operator=(Mesh&& other) noexcept;

        ~Mesh();

        void draw(const ShaderProgram& shaderProgram) const;

    private:
        vertex_container_type m_vertices;
        index_container_type m_indices;
        texture_container_type m_textures;

        handle_type m_vertexArrayObject{ 0 };
        handle_type m_vertexBufferObject{ 0 };
        handle_type m_elementBufferObject{ 0 };

        const Model* m_parent{ nullptr };

        void setupMesh();

        static constexpr auto elementSizeOf(std::ranges::sized_range auto&& range) noexcept;

        static constexpr auto sizeOf(std::ranges::sized_range auto&& range) noexcept;

        [[nodiscard]] std::vector<texture_type> loadMaterialTextures(const aiMaterial* material,
                                                                     aiTextureType assimpTextureType) const;

        [[nodiscard]] static handle_type loadTextureFromFile(const std::filesystem::path& path);
    };

    constexpr auto Mesh::elementSizeOf(std::ranges::sized_range auto&& range) noexcept
    {
        return sizeof(std::ranges::range_value_t<decltype(range)>);
    }

    constexpr auto Mesh::sizeOf(std::ranges::sized_range auto&& range) noexcept
    {
        return std::ranges::size(range) * elementSizeOf(range);
    }

    constexpr Texture::Type from(const aiTextureType assimpTextureType)
    {
        switch (assimpTextureType)
        {
            case aiTextureType_DIFFUSE:
                return Texture::Type::DIFFUSE;
            case aiTextureType_SPECULAR:
                return Texture::Type::SPECULAR;
            case aiTextureType_NORMALS:
                return Texture::Type::NORMAL;
            case aiTextureType_HEIGHT:
                return Texture::Type::HEIGHT;
            default:
                throw std::runtime_error("unsupported texture type");
        }
    }

    constexpr auto gl_cast(const PixelFormat pixelFormat)
    {
        switch (pixelFormat)
        {
            case PixelFormat::Red:
                return GL_RED;
            case PixelFormat::RG:
                return GL_RG;
            case PixelFormat::RGB:
                return GL_RGB;
            case PixelFormat::RGBA:
                return GL_RGBA;
            default:
                throw std::runtime_error("Unsupported pixel format");
        }
    }
} // lgl

template<>
struct std::formatter<lgl::Texture::Type> : std::formatter<std::string_view>
{
    constexpr auto parse(std::format_parse_context& ctx)
    {
        return std::formatter<std::string_view>::parse(ctx);
    }

    template<typename FormatContext>
    auto format(const lgl::Texture::Type type, FormatContext& ctx) const
    {
        std::string_view name{};
        switch (type)
        {
            case lgl::Texture::Type::DIFFUSE:
                name = "diffuse";
                break;
            case lgl::Texture::Type::SPECULAR:
                name = "specular";
                break;
            case lgl::Texture::Type::NORMAL:
                name = "normal";
                break;
            case lgl::Texture::Type::HEIGHT:
                name = "height";
                break;
            default:
                throw std::runtime_error{ "Unknown texture type" };
        }
        return std::formatter<std::string_view>::format(name, ctx);
    }
};

#endif //LEARNOPENGL_APP_MESH_H
