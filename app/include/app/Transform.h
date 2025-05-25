//
// Created by user on 5/25/25.
//

#ifndef LEARNOPENGL_APP_TRANSFORM_H
#define LEARNOPENGL_APP_TRANSFORM_H

#include "app/constants.h"

#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>

#include <vector>


namespace lgl
{
    // Transform component for hierarchical transformations
    class Transform
    {
    public:
        Transform();
        explicit Transform(const glm::vec3& position,
                           const glm::quat& rotation = { 1.0f, 0.0f, 0.0f, 0.0f },
                           const glm::vec3& scale = glm::vec3{ 1.0f });

        // Local transformations
        void setLocalPosition(const glm::vec3& position);
        void setLocalRotation(const glm::quat& rotation);
        void setLocalScale(const glm::vec3& scale);

        [[nodiscard]] const glm::vec3& getLocalPosition() const;
        [[nodiscard]] const glm::quat& getLocalRotation() const;
        [[nodiscard]] const glm::vec3& getLocalScale() const;

        // World transformations
        [[nodiscard]] glm::vec3 getWorldPosition() const;
        [[nodiscard]] glm::quat getWorldRotation() const;
        [[nodiscard]] glm::vec3 getWorldScale() const;

        void setWorldPosition(const glm::vec3& position);
        void setWorldRotation(const glm::quat& rotation);

        // Hierarchy
        void setParent(Transform* parent);
        [[nodiscard]] Transform* getParent() const;
        void addChild(Transform* child);
        void removeChild(const Transform* child);
        [[nodiscard]] const std::vector<Transform*>& getChildren() const;

        // Matrices
        [[nodiscard]] glm::mat4 getLocalMatrix() const;
        [[nodiscard]] glm::mat4 getWorldMatrix() const;

        // Direction vectors in world space
        [[nodiscard]] glm::vec3 forward() const;
        [[nodiscard]] glm::vec3 up() const;
        [[nodiscard]] glm::vec3 right() const;

        // Utility methods
        void lookAt(const glm::vec3& target, const glm::vec3& up = constants::WORLD_UP);

    private:
        glm::vec3 m_localPosition;
        glm::quat m_localRotation;
        glm::vec3 m_localScale;

        Transform* m_parent;
        std::vector<Transform*> m_children;

        mutable bool m_worldMatrixDirty;
        mutable glm::mat4 m_worldMatrix{};

        void markDirty() const;
        void updateWorldMatrix() const;
    };
} // lgl

#endif //LEARNOPENGL_APP_TRANSFORM_H
