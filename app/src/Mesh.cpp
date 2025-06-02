//
// Created by user on 6/1/25.
//

#include "app/Mesh.h"

#include <ranges>
#include "app/Model.h"

glm::vec3 from(const aiVector3D& vector)
{
    return glm::vec3{ vector.x, vector.y, vector.z };
}

namespace lgl
{
    Mesh::Mesh(vertex_container_type vertices, index_container_type indices, texture_container_type textures)
        : m_vertices{ std::move(vertices) },
          m_indices{ std::move(indices) },
          m_textures{ std::move(textures) }
    {
        setupMesh();
    }

    Mesh::Mesh(const Model& model, const aiMesh* mesh, const aiScene* scene)
        : m_parent{ &model }
    {
        for (std::size_t i{ 0 }; i < mesh->mNumVertices; ++i)
        {
            m_vertices.emplace_back(from(mesh->mVertices[i]),
                                    mesh->HasNormals()
                                    ? from(mesh->mNormals[i])
                                    : glm::vec3{ 0.0f, 0.0f, 0.0f },
                                    mesh->mTextureCoords[0] != nullptr
                                    ? from(mesh->mTextureCoords[0][i])
                                    : glm::vec2{ 0.0f, 0.0f },
                                    mesh->mTextureCoords[0] != nullptr && mesh->mTangents != nullptr
                                    ? from(mesh->mTangents[i])
                                    : glm::vec3{ 0.0f, 0.0f, 0.0f },
                                    mesh->mTextureCoords[0] != nullptr && mesh->mBitangents != nullptr
                                    ? from(mesh->mBitangents[i])
                                    : glm::vec3{ 0.0f, 0.0f, 0.0f });
        }

        for (std::size_t i{ 0 }; i < mesh->mNumFaces; ++i)
        {
            const auto face{ mesh->mFaces[i] };
            for (std::size_t j{ 0 }; j < face.mNumIndices; ++j)
            {
                m_indices.emplace_back(face.mIndices[j]);
            }
        }

        const auto material{ scene->mMaterials[mesh->mMaterialIndex] };
        m_textures.append_range(loadMaterialTextures(material, aiTextureType_DIFFUSE));
        m_textures.append_range(loadMaterialTextures(material, aiTextureType_SPECULAR));
        m_textures.append_range(loadMaterialTextures(material, aiTextureType_NORMALS));
        m_textures.append_range(loadMaterialTextures(material, aiTextureType_HEIGHT));

        if (m_textures.empty())
        {
            std::println(stderr, "No texture is loaded");
        }

        setupMesh();
    }

    Mesh::Mesh(Mesh&& other) noexcept
        : m_vertices{ std::move(other.m_vertices) },
          m_indices{ std::move(other.m_indices) },
          m_textures{ std::move(other.m_textures) },
          m_vertexArrayObject{ other.m_vertexArrayObject },
          m_vertexBufferObject{ other.m_vertexBufferObject },
          m_elementBufferObject{ other.m_elementBufferObject }
    {
        // Invalidate the moved-from object's handles
        other.m_vertexArrayObject = 0;
        other.m_vertexBufferObject = 0;
        other.m_elementBufferObject = 0;
    }

    Mesh& Mesh::operator=(Mesh&& other) noexcept
    {
        if (this == &other)
            return *this;
        m_vertices = std::move(other.m_vertices);
        m_indices = std::move(other.m_indices);
        m_textures = std::move(other.m_textures);
        m_vertexArrayObject = other.m_vertexArrayObject;
        m_vertexBufferObject = other.m_vertexBufferObject;
        m_elementBufferObject = other.m_elementBufferObject;

        // Invalidate the moved-from object's handles
        other.m_vertexArrayObject = 0;
        other.m_vertexBufferObject = 0;
        other.m_elementBufferObject = 0;

        return *this;
    }

    Mesh::~Mesh()
    {
        glDeleteVertexArrays(1, &m_vertexArrayObject);
        glDeleteBuffers(1, &m_vertexBufferObject);
        glDeleteBuffers(1, &m_elementBufferObject);
    }

    void Mesh::draw(const ShaderProgram& shaderProgram) const
    {
        std::array<std::size_t, static_cast<std::size_t>(Texture::Type::Size)> offsets{};
        for (auto&& [index, texture] : std::views::enumerate(m_textures))
        {
            glActiveTexture(GL_TEXTURE0 + index);

            shaderProgram.setUniform(std::format("material.{}s[{}]",
                                                 texture.type,
                                                 offsets[static_cast<std::size_t>(texture.type)]++),
                                     static_cast<GLuint>(index)
            );
            glBindTexture(GL_TEXTURE_2D, texture.id);
        }
        glActiveTexture(GL_TEXTURE0);

        glBindVertexArray(m_vertexArrayObject);
        glDrawElements(
            GL_TRIANGLES,
            static_cast<GLsizei>(m_indices.size()),
            GL_UNSIGNED_INT,
            nullptr
        );
        glBindVertexArray(0);
    }

    std::unordered_map<std::filesystem::path, Mesh::handle_type> Mesh::s_textureCache{};

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

        glEnableVertexAttribArray(3);
        glVertexAttribPointer(
            3,
            3,
            GL_FLOAT,
            GL_FALSE,
            static_cast<GLsizei>(elementSizeOf(m_vertices)),
            reinterpret_cast<const GLvoid*>(offsetof(vertex_type, tangent))
        );

        glEnableVertexAttribArray(4);
        glVertexAttribPointer(
            4,
            3,
            GL_FLOAT,
            GL_FALSE,
            static_cast<GLsizei>(elementSizeOf(m_vertices)),
            reinterpret_cast<const GLvoid*>(offsetof(vertex_type, bitangent))
        );

        glEnableVertexAttribArray(5);
        glVertexAttribIPointer(
            5,
            MAX_BONE_INFLUENCE,
            GL_INT,
            static_cast<GLsizei>(elementSizeOf(m_vertices)),
            reinterpret_cast<const GLvoid*>(offsetof(vertex_type, boneIds))
        );

        glEnableVertexAttribArray(6);
        glVertexAttribPointer(
            6,
            MAX_BONE_INFLUENCE,
            GL_FLOAT,
            GL_FALSE,
            static_cast<GLsizei>(elementSizeOf(m_vertices)),
            reinterpret_cast<const GLvoid*>(offsetof(vertex_type, weights))
        );

        glBindVertexArray(0);
    }

    std::vector<Mesh::texture_type> Mesh::loadMaterialTextures(const aiMaterial* material,
                                                               const aiTextureType assimpTextureType) const
    {
        std::vector<texture_type> textures{};
        for (std::size_t i{ 0 }; i < material->GetTextureCount(assimpTextureType); ++i)
        {
            aiString textureFilename{};
            material->GetTexture(assimpTextureType, i, &textureFilename);
            std::string texturePath{ textureFilename.C_Str() };
#ifndef _WIN32
            std::ranges::replace(texturePath, '\\', '/');
#endif
            const auto path{
                std::filesystem::canonical(m_parent->directory() / texturePath)
               .make_preferred()
               .lexically_normal()
            };
            textures.emplace_back(loadTextureFromFile(path),
                                  from(assimpTextureType),
                                  path);
        }
        return textures;
    }


    Mesh::handle_type Mesh::loadTextureFromFile(const std::filesystem::path& path)
    {
        // Check if the texture is already in the cache
        if (s_textureCache.contains(path))
        {
            return s_textureCache.at(path);
        }

        // Texture not in cache, load it
        const auto image{ loadImage(path).flipVertically() };

        GLuint mapId{};
        glGenTextures(1, &mapId);
        glBindTexture(GL_TEXTURE_2D, mapId);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        const auto format{ gl_cast(image.pixelFormat) };

        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            format,
            static_cast<GLsizei>(image.width),
            static_cast<GLsizei>(image.height),
            0,
            format,
            GL_UNSIGNED_BYTE,
            image.span().data()
        );
        glGenerateMipmap(GL_TEXTURE_2D);

        glBindTexture(GL_TEXTURE_2D, 0);

        // Store the texture in the cache
        s_textureCache.emplace(path, mapId);

        return mapId;
    }

    void Mesh::clearTextureCache()
    {
        for (auto textureId : s_textureCache | std::views::values)
        {
            glDeleteTextures(1, &textureId);
        }
        s_textureCache.clear();
    }
} // lgl
