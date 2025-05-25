//
// Created by user on 5/25/25.
//

#ifndef LEARNOPENGL_APP_SPATIAL_H
#define LEARNOPENGL_APP_SPATIAL_H

#include "app/constants.h"
#include "app/Transform.h"

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

#include <vector>

namespace lgl
{
    // Base class for objects with position and orientation in 3D space
    class Spatial
    {
    public:
        // Constructor with default position and orientation
        explicit Spatial(const glm::vec3& position = constants::DEFAULT_POSITION,
                         float yaw = constants::DEFAULT_YAW,
                         float pitch = constants::DEFAULT_PITCH,
                         float roll = constants::DEFAULT_ROLL,
                         const glm::vec3& worldUp = constants::WORLD_UP);

        // Copy/move constructors and assignment operators
        Spatial(const Spatial&) = default;
        Spatial(Spatial&&) noexcept = default;
        Spatial& operator=(const Spatial&) = default;
        Spatial& operator=(Spatial&&) noexcept = default;

        virtual ~Spatial() = default;

        // Transform methods
        [[nodiscard]] Transform& getTransform();
        [[nodiscard]] const Transform& getTransform() const;

        // Legacy position methods (delegates to transform)
        void setPosition(const glm::vec3& position);
        [[nodiscard]] const glm::vec3& getPosition() const;

        // Legacy orientation methods (delegates to transform)
        void setOrientation(float yaw, float pitch, float roll = 0.0f);
        [[nodiscard]] float getYaw() const;
        [[nodiscard]] float getPitch() const;
        [[nodiscard]] float getRoll() const;

        // Legacy direction vectors (delegates to transform)
        [[nodiscard]] glm::vec3 getForward() const;
        [[nodiscard]] glm::vec3 getUp() const;
        [[nodiscard]] glm::vec3 getRight() const;
        void setWorldUp(const glm::vec3& up);

        // Movement methods
        void moveForward(float distance);
        void moveRight(float distance);
        void moveUp(float distance);
        void moveInDirection(const glm::vec3& direction, float distance);

        [[nodiscard]] float movementSpeed() const;
        void setMovementSpeed(float movementSpeed);

        // Rotation methods
        void rotate(float yawOffset, float pitchOffset);
        void rotateAround(const glm::vec3& point, const glm::vec3& axis, float angle);

        // Matrix transformation
        [[nodiscard]] glm::mat4 getModelMatrix() const;

        // Path following
        void followPath(const std::vector<glm::vec3>& path, float t);

        // Interpolation
        void interpolateTo(const Spatial& target, float t);

    protected:
        // Core spatial properties
        Transform m_transform;
        float m_movementSpeed{ constants::DEFAULT_MOVEMENT_SPEED };

        // Euler angles cache (for legacy support)
        struct EulerAngles
        {
            float yaw{ constants::DEFAULT_YAW };
            float pitch{ constants::DEFAULT_PITCH };
            float roll{ constants::DEFAULT_ROLL };
        };

        EulerAngles m_eulerAngles;

        // Update Euler angles from quaternion
        void updateEulerAngles();

        // Update quaternion from Euler angles
        void updateQuaternion();

        // Notify derived classes that spatial state has changed
        virtual void onSpatialChanged();
    };
} // lgl

#endif //LEARNOPENGL_APP_SPATIAL_H
