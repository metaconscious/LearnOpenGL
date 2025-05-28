//
// Created by user on 5/28/25.
//

#ifndef LEARNOPENGL_APP_CAMERA_H
#define LEARNOPENGL_APP_CAMERA_H

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <glm/gtc/quaternion.hpp>

namespace lgl
{
    class Camera
    {
    public:
        virtual ~Camera() = default;
        // Core required interfaces
        [[nodiscard]] virtual glm::mat4 getViewMatrix() const = 0;
        [[nodiscard]] virtual glm::mat4 getProjectionMatrix() const = 0;
        // Position methods
        virtual void setPosition(const glm::vec3& position) = 0;
        [[nodiscard]] virtual glm::vec3 getPosition() const = 0;

        // Rotation methods (quaternion-based)
        virtual void setRotation(const glm::quat& rotation) = 0;
        [[nodiscard]] virtual glm::quat getRotation() const = 0;

        // Euler angle support
        virtual void setEulerAngles(const glm::vec3& eulerAngles) = 0;
        [[nodiscard]] virtual glm::vec3 getEulerAngles() const = 0;

        // Direction vectors
        [[nodiscard]] virtual glm::vec3 getForwardVector() const = 0;
        [[nodiscard]] virtual glm::vec3 getRightVector() const = 0;
        [[nodiscard]] virtual glm::vec3 getUpVector() const = 0;
    };
} // lgl

#endif //LEARNOPENGL_APP_CAMERA_H
