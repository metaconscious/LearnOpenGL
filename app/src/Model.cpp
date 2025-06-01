//
// Created by user on 6/1/25.
//

#include "app/Model.h"

#include <algorithm>
#include <print>
#include <utility>
#include <assimp/postprocess.h>

namespace lgl
{
    Model Model::load(const std::filesystem::path& path)
    {
        if (!std::filesystem::exists(path))
        {
            std::println(stderr, "'{}' not exist", path.string());
            throw std::runtime_error("Not found");
        }

        if (!std::filesystem::is_regular_file(path))
        {
            std::println(stderr, "'{}' not a regular file", path.string());
            throw std::runtime_error("Not a regular file");
        }

        Assimp::Importer importer{};
        const auto scene{
            importer.ReadFile(std::filesystem::absolute(path).c_str(),
                              aiProcess_Triangulate | aiProcess_FlipUVs)
        };

        if (scene == nullptr || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || scene->mRootNode == nullptr)
        {
            std::println(stderr, "[Assimp::Importer] {}", importer.GetErrorString());
            throw std::runtime_error("Failed to load model");
        }

        Model model{ std::filesystem::absolute(path.parent_path()) };
        model.processNodes(scene->mRootNode, scene);
        return model;
    }

    void Model::draw(const ShaderProgram& shaderProgram) const
    {
        std::ranges::for_each(m_meshes,
                              [&shaderProgram](auto&& mesh)
                              {
                                  mesh.draw(shaderProgram);
                              });
    }

    Model::Model(Model&& other) noexcept
        : m_meshes{ std::move(other.m_meshes) },
          m_directory{ std::move(other.m_directory) }
    {
    }

    Model& Model::operator=(Model&& other) noexcept
    {
        if (this == &other)
            return *this;
        m_meshes = std::move(other.m_meshes);
        m_directory = std::move(other.m_directory);
        return *this;
    }

    const std::filesystem::path& Model::directory() const noexcept
    {
        return m_directory;
    }

    Model::Model(std::filesystem::path directory)
        : m_directory{ std::move(directory) }
    {
    }

    void Model::processNodes(const aiNode* node, const aiScene* scene) // NOLINT(*-no-recursion)
    {
        for (std::size_t i{ 0 }; i < node->mNumMeshes; ++i)
        {
            const auto mesh{ scene->mMeshes[node->mMeshes[i]] };
            m_meshes.emplace_back(*this, mesh, scene);
        }
        for (std::size_t i{ 0 }; i < node->mNumChildren; ++i)
        {
            processNodes(node->mChildren[i], scene);
        }
    }
} // lgl
