//
// Created by user on 6/1/25.
//

#ifndef LEARNOPENGL_APP_MODEL_H
#define LEARNOPENGL_APP_MODEL_H

#include <vector>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>

#include "app/Mesh.h"
#include "app/ShaderProgram.h"

namespace lgl
{
    class Model
    {
    public:
        [[nodiscard]] static Model load(const std::filesystem::path& path);

        void draw(const ShaderProgram& shaderProgram) const;

        Model(const Model& other) = delete;

        Model(Model&& other) noexcept;

        Model& operator=(const Model& other) = delete;

        Model& operator=(Model&& other) noexcept;

        [[nodiscard]] const std::filesystem::path& directory() const noexcept;

    private:
        explicit Model(std::filesystem::path directory);

        void processNodes(const aiNode* node, const aiScene* scene);

        std::vector<Mesh> m_meshes;
        std::filesystem::path m_directory;
    };
} // lgl

#endif //LEARNOPENGL_APP_MODEL_H
