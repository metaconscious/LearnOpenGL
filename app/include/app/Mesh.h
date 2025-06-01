//
// Created by user on 6/1/25.
//

#ifndef LEARNOPENGL_APP_MESH_H
#define LEARNOPENGL_APP_MESH_H

#include <format>
#include <string_view>
#include <vector>
#include <glad/glad.h>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include "app/ShaderProgram.h"

namespace lgl
{
    struct Vertex
    {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 textureCoordinates;
    };

    struct Texture
    {
        enum class Type
        {
            DIFFUSE = 0,
            SPECULAR,
            Size
        };

        GLuint id;
        Type type;
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

        void draw(const ShaderProgram& shaderProgram);

    private:
        vertex_container_type m_vertices;
        index_container_type m_indices;
        texture_container_type m_textures;

        handle_type m_vertexArrayObject{ 0 };
        handle_type m_vertexBufferObject{ 0 };
        handle_type m_elementBufferObject{ 0 };

        void setupMesh();

        static constexpr auto elementSizeOf(std::ranges::sized_range auto&& range) noexcept;

        static constexpr auto sizeOf(std::ranges::sized_range auto&& range) noexcept;
    };

    constexpr auto Mesh::elementSizeOf(std::ranges::sized_range auto&& range) noexcept
    {
        return sizeof(std::ranges::range_value_t<decltype(range)>);
    }

    constexpr auto Mesh::sizeOf(std::ranges::sized_range auto&& range) noexcept
    {
        return std::ranges::size(range) * elementSizeOf(range);
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
            default:
                throw std::runtime_error{ "Unknown texture type" };
        }
        return std::formatter<std::string_view>::format(name, ctx);
    }
};

#endif //LEARNOPENGL_APP_MESH_H
