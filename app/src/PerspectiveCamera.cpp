//
// Created by user on 5/28/25.
//

#include "app/PerspectiveCamera.h"

#include <glm/gtc/matrix_transform.hpp>

namespace lgl
{
    PerspectiveCamera::PerspectiveCamera(const float fov,
                                         const float aspectRatio,
                                         const float nearPlane,
                                         const float farPlane)
        : m_fov{ fov },
          m_aspectRatio{ aspectRatio },
          m_near{ nearPlane },
          m_far{ farPlane }
    {
    }

    glm::mat4 PerspectiveCamera::getViewMatrix() const
    {
        if (m_viewDirty)
        {
            const glm::vec3 forward = getForwardVector();
            m_viewMatrix = glm::lookAt(m_position, m_position + forward, getUpVector());
            m_viewDirty = false;
        }
        return m_viewMatrix;
    }

    glm::mat4 PerspectiveCamera::getProjectionMatrix() const
    {
        if (m_projectionDirty)
        {
            m_projectionMatrix = glm::perspective(glm::radians(m_fov), m_aspectRatio, m_near, m_far);
            m_projectionDirty = false;
        }
        return m_projectionMatrix;
    }

    void PerspectiveCamera::setPosition(const glm::vec3& position)
    {
        m_position = position;
        m_viewDirty = true;
    }

    glm::vec3 PerspectiveCamera::getPosition() const
    {
        return m_position;
    }

    void PerspectiveCamera::setRotation(const glm::quat& rotation)
    {
        m_rotation = glm::normalize(rotation);
        m_viewDirty = true;
    }

    glm::quat PerspectiveCamera::getRotation() const
    {
        return m_rotation;
    }

    void PerspectiveCamera::setEulerAngles(const glm::vec3& eulerAngles)
    {
        // Convert Euler angles to quaternion (YXZ order: yaw, pitch, roll)
        setRotation({
            {
                glm::radians(eulerAngles.x), // pitch
                glm::radians(eulerAngles.y), // yaw
                glm::radians(eulerAngles.z) // roll
            }
        });
    }

    glm::vec3 PerspectiveCamera::getEulerAngles() const
    {
        // Extract Euler angles from quaternion
        return glm::degrees(glm::eulerAngles(m_rotation));
    }

    glm::vec3 PerspectiveCamera::getForwardVector() const
    {
        // Apply quaternion rotation to the forward vector (-Z axis)
        return m_rotation * glm::vec3{ 0.0f, 0.0f, -1.0f };
    }

    glm::vec3 PerspectiveCamera::getRightVector() const
    {
        // Apply quaternion rotation to the right vector (X axis)
        return m_rotation * glm::vec3{ 1.0f, 0.0f, 0.0f };
    }

    glm::vec3 PerspectiveCamera::getUpVector() const
    {
        // Apply quaternion rotation to the up vector (Y axis)
        return m_rotation * glm::vec3{ 0.0f, 1.0f, 0.0f };
    }

    void PerspectiveCamera::setPerspective(const float fov,
                                           const float aspectRatio,
                                           const float nearPlane,
                                           const float farPlane)
    {
        m_fov = fov;
        m_aspectRatio = aspectRatio;
        m_near = nearPlane;
        m_far = farPlane;
        m_projectionDirty = true;
    }

    void PerspectiveCamera::setAspectRatio(const float aspectRatio)
    {
        if (m_aspectRatio != aspectRatio)
        {
            m_aspectRatio = aspectRatio;
            m_projectionDirty = true;
        }
    }

    float PerspectiveCamera::getFov() const
    {
        return m_fov;
    }

    void PerspectiveCamera::setFov(const float fov)
    {
        if (m_fov != fov)
        {
            m_fov = fov;
            m_projectionDirty = true;
        }
    }

    float PerspectiveCamera::getNearPlane() const
    {
        return m_near;
    }

    float PerspectiveCamera::getFarPlane() const
    {
        return m_far;
    }
} // lgl
