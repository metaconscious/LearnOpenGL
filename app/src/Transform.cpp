//
// Created by user on 5/25/25.
//

#include "app/Transform.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

namespace lgl
{
    // Transform implementation
    Transform::Transform()
        : Transform{
            glm::vec3{ 0.0f },
            glm::quat{ 1.0f, 0.0f, 0.0f, 0.0f },
            glm::vec3{ 1.0f }
        }
    {
    }

    Transform::Transform(const glm::vec3& position, const glm::quat& rotation, const glm::vec3& scale)
        : m_localPosition{ position },
          m_localRotation{ rotation },
          m_localScale{ scale },
          m_parent{ nullptr },
          m_worldMatrixDirty{ true }
    {
    }

    void Transform::setLocalPosition(const glm::vec3& position)
    {
        m_localPosition = position;
        markDirty();
    }

    void Transform::setLocalRotation(const glm::quat& rotation)
    {
        m_localRotation = rotation;
        markDirty();
    }

    void Transform::setLocalScale(const glm::vec3& scale)
    {
        m_localScale = scale;
        markDirty();
    }

    const glm::vec3& Transform::getLocalPosition() const
    {
        return m_localPosition;
    }

    const glm::quat& Transform::getLocalRotation() const
    {
        return m_localRotation;
    }

    const glm::vec3& Transform::getLocalScale() const
    {
        return m_localScale;
    }

    glm::vec3 Transform::getWorldPosition() const
    {
        if (m_parent == nullptr)
        {
            return m_localPosition;
        }

        return getWorldMatrix() * glm::vec4{ 0.0f, 0.0f, 0.0f, 1.0f };
    }

    glm::quat Transform::getWorldRotation() const
    {
        if (m_parent == nullptr)
        {
            return m_localRotation;
        }

        // Extract rotation from world matrix
        return glm::quat_cast(getWorldMatrix());
    }

    glm::vec3 Transform::getWorldScale() const
    {
        if (m_parent == nullptr)
        {
            return m_localScale;
        }

        // Extract scale from world matrix (this is approximate)
        const auto& worldMatrix{ getWorldMatrix() };
        return glm::vec3(
            glm::length(glm::vec3(worldMatrix[0])),
            glm::length(glm::vec3(worldMatrix[1])),
            glm::length(glm::vec3(worldMatrix[2]))
        );
    }

    void Transform::setWorldPosition(const glm::vec3& position)
    {
        if (m_parent == nullptr)
        {
            m_localPosition = position;
        }
        else
        {
            // Convert world position to local space
            const auto localPos{ glm::inverse(m_parent->getWorldMatrix()) * glm::vec4{ position, 1.0f } };
            m_localPosition = glm::vec3{ localPos };
        }
        markDirty();
    }

    void Transform::setWorldRotation(const glm::quat& rotation)
    {
        if (m_parent == nullptr)
        {
            m_localRotation = rotation;
        }
        else
        {
            // Convert world rotation to local space
            m_localRotation = glm::inverse(m_parent->getWorldRotation()) * rotation;
        }
        markDirty();
    }

    void Transform::setParent(Transform* parent)
    {
        if (m_parent == parent)
        {
            return;
        }

        // Remove from old parent's children list
        if (m_parent != nullptr)
        {
            m_parent->removeChild(this);
        }

        // Store world transform before changing parent
        const auto worldPos{ getWorldPosition() };
        const auto worldRot{ getWorldRotation() };

        // Set new parent
        m_parent = parent;

        // Add to new parent's children list
        if (m_parent != nullptr)
        {
            m_parent->addChild(this);

            // Maintain world transform
            setWorldPosition(worldPos);
            setWorldRotation(worldRot);
        }

        markDirty();
    }

    Transform* Transform::getParent() const
    {
        return m_parent;
    }

    void Transform::addChild(Transform* child)
    {
        if (child == nullptr || child == this)
            return;

        // Check if already a child
        if (const auto it{ std::ranges::find(m_children, child) }; it == m_children.end())
        {
            m_children.push_back(child);
        }
    }

    void Transform::removeChild(const Transform* child)
    {
        if (const auto it{ std::ranges::find(m_children, child) }; it != m_children.end())
        {
            m_children.erase(it);
        }
    }

    const std::vector<Transform*>& Transform::getChildren() const
    {
        return m_children;
    }

    glm::mat4 Transform::getLocalMatrix() const
    {
        // Build TRS matrix
        const glm::mat4 translationMatrix{ glm::translate(glm::mat4{ 1.0f }, m_localPosition) };
        const glm::mat4 rotationMatrix{ glm::toMat4(m_localRotation) };
        const glm::mat4 scaleMatrix{ glm::scale(glm::mat4{ 1.0f }, m_localScale) };

        return translationMatrix * rotationMatrix * scaleMatrix;
    }

    glm::mat4 Transform::getWorldMatrix() const
    {
        if (m_worldMatrixDirty)
        {
            updateWorldMatrix();
        }
        return m_worldMatrix;
    }

    glm::vec3 Transform::forward() const
    {
        return getWorldRotation() * constants::WORLD_FORWARD;
    }

    glm::vec3 Transform::up() const
    {
        return getWorldRotation() * constants::WORLD_UP;
    }

    glm::vec3 Transform::right() const
    {
        return getWorldRotation() * constants::WORLD_RIGHT;
    }

    void Transform::lookAt(const glm::vec3& target, const glm::vec3& up)
    {
        const auto worldPos{ getWorldPosition() };
        const auto direction{ glm::normalize(target - worldPos) };

        // Create look-at rotation
        const auto right{ glm::normalize(glm::cross(direction, up)) };
        const auto upDir{ glm::cross(right, direction) };

        const glm::mat3 rotationMatrix{ right, upDir, -direction };
        setWorldRotation(glm::quat_cast(rotationMatrix));
    }

    void Transform::markDirty() const
    {
        m_worldMatrixDirty = true;

        // Mark all children as dirty
        for (const auto* child : m_children)
        {
            child->markDirty();
        }
    }

    void Transform::updateWorldMatrix() const
    {
        if (m_parent == nullptr)
        {
            m_worldMatrix = getLocalMatrix();
        }
        else
        {
            m_worldMatrix = m_parent->getWorldMatrix() * getLocalMatrix();
        }
        m_worldMatrixDirty = false;
    }
} // lgl
