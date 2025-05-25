//
// Created by user on 5/25/25.
//

#include "app/Spatial.h"

#include <glm/ext/quaternion_common.hpp>
#include <glm/ext/quaternion_trigonometric.hpp>
#include <glm/gtc/quaternion.hpp>

#include <algorithm>

namespace lgl
{
    // Spatial implementation
    Spatial::Spatial(const glm::vec3& position,
                     const float yaw,
                     const float pitch,
                     const float roll,
                     const glm::vec3& worldUp)
        : m_eulerAngles{ yaw, pitch, roll }
    {
        // Set up transform
        m_transform.setLocalPosition(position);

        // Convert Euler angles to quaternion
        updateQuaternion();
    }

    Transform& Spatial::getTransform()
    {
        return m_transform;
    }

    const Transform& Spatial::getTransform() const
    {
        return m_transform;
    }

    void Spatial::updateEulerAngles()
    {
        // Extract Euler angles from quaternion
        const auto eulerAngles{ glm::eulerAngles(m_transform.getLocalRotation()) };
        m_eulerAngles.yaw = glm::degrees(eulerAngles.y);
        m_eulerAngles.pitch = glm::degrees(eulerAngles.x);
        m_eulerAngles.roll = glm::degrees(eulerAngles.z);
    }

    void Spatial::updateQuaternion()
    {
        // Convert Euler angles to quaternion
        const glm::quat quat{
            glm::vec3(
                glm::radians(m_eulerAngles.pitch),
                glm::radians(m_eulerAngles.yaw),
                glm::radians(m_eulerAngles.roll)
            )
        };
        m_transform.setLocalRotation(quat);

        onSpatialChanged();
    }

    void Spatial::onSpatialChanged()
    {
        // Base implementation does nothing
    }

    void Spatial::setPosition(const glm::vec3& position)
    {
        m_transform.setLocalPosition(position);
        onSpatialChanged();
    }

    const glm::vec3& Spatial::getPosition() const
    {
        return m_transform.getLocalPosition();
    }

    void Spatial::setOrientation(const float yaw, const float pitch, const float roll)
    {
        m_eulerAngles.yaw = yaw;
        m_eulerAngles.pitch = std::clamp(pitch, constants::MIN_PITCH, constants::MAX_PITCH);
        m_eulerAngles.roll = roll;
        updateQuaternion();
    }

    float Spatial::getYaw() const
    {
        return m_eulerAngles.yaw;
    }

    float Spatial::getPitch() const
    {
        return m_eulerAngles.pitch;
    }

    float Spatial::getRoll() const
    {
        return m_eulerAngles.roll;
    }

    glm::vec3 Spatial::getForward() const
    {
        return m_transform.forward();
    }

    glm::vec3 Spatial::getUp() const
    {
        return m_transform.up();
    }

    glm::vec3 Spatial::getRight() const
    {
        return m_transform.right();
    }

    void Spatial::setWorldUp(const glm::vec3& up)
    {
        // Recalculate orientation to maintain forward direction but use new up
        const auto forward{ getForward() };
        const auto right{ glm::normalize(glm::cross(forward, glm::normalize(up))) };
        const auto newUp{ glm::cross(right, forward) };

        const glm::mat3 rotMat{ right, newUp, -forward };
        m_transform.setLocalRotation(glm::quat_cast(rotMat));

        // Update Euler angles
        updateEulerAngles();
        onSpatialChanged();
    }

    void Spatial::moveForward(const float distance)
    {
        m_transform.setLocalPosition(m_transform.getLocalPosition() + getForward() * distance * m_movementSpeed);
        onSpatialChanged();
    }

    void Spatial::moveRight(const float distance)
    {
        m_transform.setLocalPosition(m_transform.getLocalPosition() + getRight() * distance * m_movementSpeed);
        onSpatialChanged();
    }

    void Spatial::moveUp(const float distance)
    {
        m_transform.setLocalPosition(m_transform.getLocalPosition() + getUp() * distance * m_movementSpeed);
        onSpatialChanged();
    }

    void Spatial::moveInDirection(const glm::vec3& direction, const float distance)
    {
        m_transform.setLocalPosition(
            m_transform.getLocalPosition() + glm::normalize(direction) * distance * m_movementSpeed
        );
        onSpatialChanged();
    }

    void Spatial::rotate(const float yawOffset, const float pitchOffset)
    {
        m_eulerAngles.yaw += yawOffset;
        m_eulerAngles.pitch = std::clamp(m_eulerAngles.pitch + pitchOffset,
                                         constants::MIN_PITCH,
                                         constants::MAX_PITCH);
        updateQuaternion();
    }

    void Spatial::rotateAround(const glm::vec3& point, const glm::vec3& axis, const float angle)
    {
        // Get current position relative to pivot point
        auto relativePos{ m_transform.getLocalPosition() - point };

        // Create rotation quaternion
        const auto rotation{ glm::angleAxis(glm::radians(angle), glm::normalize(axis)) };

        // Rotate position around pivot
        relativePos = rotation * relativePos;
        m_transform.setLocalPosition(point + relativePos);

        // Rotate orientation
        m_transform.setLocalRotation(rotation * m_transform.getLocalRotation());

        // Update Euler angles
        updateEulerAngles();
        onSpatialChanged();
    }

    float Spatial::movementSpeed() const
    {
        return m_movementSpeed;
    }

    void Spatial::setMovementSpeed(const float movementSpeed)
    {
        m_movementSpeed = movementSpeed;
    }

    glm::mat4 Spatial::getModelMatrix() const
    {
        return m_transform.getWorldMatrix();
    }

    void Spatial::followPath(const std::vector<glm::vec3>& path, float t)
    {
        if (path.empty())
        {
            return;
        }
        if (path.size() == 1)
        {
            setPosition(path.at(0));
            return;
        }

        // Normalize t to [0, 1]
        t = std::fmod(t, 1.0f);
        if (t < 0.0f)
        {
            t += 1.0f;
        }

        // Calculate segment
        const auto segmentLength{ 1.0f / (path.size() - 1) };
        auto segment{ static_cast<int>(t / segmentLength) };
        const auto segmentT{ (t - segment * segmentLength) / segmentLength };

        // Ensure we don't go out of bounds
        segment = std::min(segment, static_cast<int>(path.size()) - 2);

        // Linear interpolation between points
        const auto newPosition{ glm::mix(path.at(segment), path.at(segment + 1), segmentT) };
        setPosition(newPosition);

        // Look in the direction of movement if we have enough points
        if (segment + 2 < path.size())
        {
            const auto lookTarget{ glm::mix(path.at(segment + 1), path.at(segment + 2), segmentT) };
            m_transform.lookAt(lookTarget);
            updateEulerAngles();
        }
    }

    void Spatial::interpolateTo(const Spatial& target, float t)
    {
        t = std::clamp(t, 0.0f, 1.0f);

        // Interpolate position
        m_transform.setLocalPosition(glm::mix(m_transform.getLocalPosition(),
                                              target.m_transform.getLocalPosition(), t));

        // Interpolate rotation (using slerp for quaternions)
        m_transform.setLocalRotation(glm::slerp(m_transform.getLocalRotation(),
                                                target.m_transform.getLocalRotation(), t));

        // Update Euler angles
        updateEulerAngles();
        onSpatialChanged();
    }
} // lgl
