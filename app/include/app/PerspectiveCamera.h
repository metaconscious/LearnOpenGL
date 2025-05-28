//
// Created by user on 5/28/25.
//

#ifndef LEARNOPENGL_APP_PERSPECTIVECAMERA_H
#define LEARNOPENGL_APP_PERSPECTIVECAMERA_H

#include "app/Camera.h"

namespace lgl
{
    class PerspectiveCamera final : public Camera
    {
    public:
        explicit PerspectiveCamera(float fov = 45.0f,
                                   float aspectRatio = 16.0f / 9.0f,
                                   float nearPlane = 0.1f,
                                   float farPlane = 1000.0f);

        // View matrix implementation with caching
        glm::mat4 getViewMatrix() const override;

        // Projection matrix implementation with caching
        glm::mat4 getProjectionMatrix() const override;

        void setPosition(const glm::vec3& position) override;

        glm::vec3 getPosition() const override;

        void setRotation(const glm::quat& rotation) override;

        glm::quat getRotation() const override;

        void setEulerAngles(const glm::vec3& eulerAngles) override;

        glm::vec3 getEulerAngles() const override;

        glm::vec3 getForwardVector() const override;

        glm::vec3 getRightVector() const override;

        glm::vec3 getUpVector() const override;

        void setPerspective(float fov, float aspectRatio, float nearPlane, float farPlane);

        void setAspectRatio(float aspectRatio);

        float getFov() const;

        void setFov(float fov);

        float getNearPlane() const;

        float getFarPlane() const;

    private:
        // Camera state
        glm::vec3 m_position{ 0.0f, 0.0f, 0.0f };
        glm::quat m_rotation{ 1.0f, 0.0f, 0.0f, 0.0f }; // Identity quaternion

        // Perspective parameters
        float m_fov{ 45.0f };
        float m_aspectRatio{ 16.0f / 9.0f };
        float m_near{ 0.1f };
        float m_far{ 1000.0f };

        // Cached matrices and state
        mutable bool m_viewDirty{ true };
        mutable bool m_projectionDirty{ true };
        mutable glm::mat4 m_viewMatrix{ 1.0f };
        mutable glm::mat4 m_projectionMatrix{ 1.0f };
    };
} // lgl

#endif //LEARNOPENGL_APP_PERSPECTIVECAMERA_H
