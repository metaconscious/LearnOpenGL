//
// Created by user on 6/1/25.
//

#include "app/Mesh.h"

#include <ranges>

namespace lgl
{
    Mesh::Mesh(vertex_container_type vertices, index_container_type indices, texture_container_type textures)
        : m_vertices{ std::move(vertices) },
          m_indices{ std::move(indices) },
          m_textures{ std::move(textures) }
    {
        setupMesh();
    }

    void Mesh::draw(const ShaderProgram& shaderProgram)
    {
        std::array<std::size_t, static_cast<std::size_t>(Texture::Type::Size)> offsets{};
        for (auto&& [index, texture] : std::views::enumerate(m_textures))
        {
            glActiveTexture(GL_TEXTURE0 + index);

            shaderProgram.setUniform(std::format("material.{}Textures[{}]",
                                                 texture.type,
                                                 offsets[static_cast<std::size_t>(texture.type)]++),
                                     static_cast<GLuint>(index)
            );
            glBindTexture(GL_TEXTURE_2D, texture.id);
        }
        glActiveTexture(GL_TEXTURE0);

        glBindVertexArray(m_vertexArrayObject);
        glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, nullptr);
        glBindVertexArray(0);
    }

    void Mesh::setupMesh()
    {
        glGenVertexArrays(1, &m_vertexArrayObject);
        glGenBuffers(1, &m_vertexBufferObject);
        glGenBuffers(1, &m_elementBufferObject);

        glBindVertexArray(m_vertexArrayObject);
        glBindBuffer(GL_ARRAY_BUFFER, m_vertexBufferObject);

        glBufferData(GL_ARRAY_BUFFER,
                     static_cast<GLsizeiptr>(sizeOf(m_vertices)),
                     m_vertices.data(),
                     GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_elementBufferObject);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                     static_cast<GLsizeiptr>(sizeOf(m_indices)),
                     m_indices.data(),
                     GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(
            0,
            3,
            GL_FLOAT,
            GL_FALSE,
            static_cast<GLsizei>(elementSizeOf(m_vertices)),
            reinterpret_cast<const GLvoid*>(offsetof(vertex_type, position))
        );

        glEnableVertexAttribArray(1);
        glVertexAttribPointer(
            1,
            3,
            GL_FLOAT,
            GL_FALSE,
            static_cast<GLsizei>(elementSizeOf(m_vertices)),
            reinterpret_cast<const GLvoid*>(offsetof(vertex_type, normal))
        );

        glEnableVertexAttribArray(2);
        glVertexAttribPointer(
            2,
            2,
            GL_FLOAT,
            GL_FALSE,
            static_cast<GLsizei>(elementSizeOf(m_vertices)),
            reinterpret_cast<const GLvoid*>(offsetof(vertex_type, textureCoordinates))
        );

        glBindVertexArray(0);
    }
} // lgl
