//
// Created by user on 5/23/25.
//

#include "app/Camera.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

#include <algorithm>
#include <format>
#include <functional>
#include <print>

namespace lgl
{
    Spatial::Spatial(const glm::vec3& position, float yaw, float pitch, float roll, const glm::vec3& worldUp)
        : m_state{
            .position = position,
            .worldUp = glm::normalize(worldUp),
            .yaw = yaw,
            .pitch = pitch,
            .roll = roll
        }
    {
        updateVectors();
    }

    void Spatial::updateVectors()
    {
        // Calculate the new forward vector
        m_state.forward.x = std::cos(glm::radians(m_state.yaw)) * std::cos(glm::radians(m_state.pitch));
        m_state.forward.y = std::sin(glm::radians(m_state.pitch));
        m_state.forward.z = std::sin(glm::radians(m_state.yaw)) * std::cos(glm::radians(m_state.pitch));
        m_state.forward = glm::normalize(m_state.forward);
        // Recalculate the right and up vector
        m_state.right = glm::normalize(glm::cross(m_state.forward, m_state.worldUp));
        m_state.up = glm::normalize(glm::cross(m_state.right, m_state.forward));

        // Notify derived classes
        onSpatialChanged();
    }

    void Spatial::onSpatialChanged()
    {
        // Do nothing
    }

    void Spatial::setPosition(const glm::vec3& position)
    {
        m_state.position = position;
        onSpatialChanged();
    }

    const glm::vec3& Spatial::getPosition() const
    {
        return m_state.position;
    }

    void Spatial::setOrientation(const float yaw, const float pitch, const float roll)
    {
        m_state.yaw = yaw;
        m_state.pitch = std::clamp(pitch, MIN_PITCH, MAX_PITCH);
        m_state.roll = roll;
        updateVectors();
    }

    float Spatial::getYaw() const
    {
        return m_state.yaw;
    }

    float Spatial::getPitch() const
    {
        return m_state.pitch;
    }

    float Spatial::getRoll() const
    {
        return m_state.roll;
    }

    const glm::vec3& Spatial::getForward() const
    {
        return m_state.forward;
    }

    const glm::vec3& Spatial::getUp() const
    {
        return m_state.up;
    }

    const glm::vec3& Spatial::getRight() const
    {
        return m_state.right;
    }

    void Spatial::setWorldUp(const glm::vec3& up)
    {
        m_state.worldUp = glm::normalize(up);
        updateVectors();
    }

    void Spatial::moveForward(const float distance)
    {
        m_state.position += m_state.forward * distance * m_state.movementSpeed;
        onSpatialChanged();
    }

    void Spatial::moveRight(const float distance)
    {
        m_state.position += m_state.right * distance * m_state.movementSpeed;
        onSpatialChanged();
    }

    void Spatial::moveUp(const float distance)
    {
        m_state.position += m_state.worldUp * distance * m_state.movementSpeed;
        onSpatialChanged();
    }

    void Spatial::moveInDirection(const glm::vec3& direction, const float distance)
    {
        m_state.position += glm::normalize(direction) * distance * m_state.movementSpeed;
        onSpatialChanged();
    }

    void Spatial::rotate(const float yawOffset, const float pitchOffset)
    {
        m_state.yaw += yawOffset;
        m_state.pitch += pitchOffset;

        // Constrain pitch to avoid gimbal lock
        m_state.pitch = std::clamp(m_state.pitch, -89.0f, 89.0f);

        updateVectors();
    }

    float Spatial::movementSpeed() const
    {
        return m_state.movementSpeed;
    }

    void Spatial::setMovementSpeed(const float movementSpeed)
    {
        m_state.movementSpeed = movementSpeed;
    }

    glm::mat4 Spatial::getModelMatrix() const
    {
        glm::mat4 model{ 1.0f };
        model = glm::translate(model, m_state.position);

        // Apply rotations based on Euler angles
        // Note: This is a simplified version; a more robust implementation
        // might use quaternions to avoid gimbal lock
        model = glm::rotate(model, glm::radians(m_state.yaw), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::rotate(model, glm::radians(m_state.pitch), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::rotate(model, glm::radians(m_state.roll), glm::vec3(0.0f, 0.0f, 1.0f));

        return model;
    }


    const Camera::CameraSettings Camera::DEFAULT_CAMERA_SETTINGS{};

    void Camera::onSpatialChanged()
    {
        m_cache->viewDirty = true;
    }

    Camera::Camera(const CameraSettings& settings)
        : Spatial{ { 0.0f, 0.0f, 3.0f }, -90.0f, 0.0f }, // Initialize base class
          m_settings{ settings },
          m_cache{ std::make_unique<Cache>() }
    {
    }

    Camera::Camera(const Camera& other)
        : Spatial{ other },
          m_settings{ other.m_settings },
          m_target{ other.m_target },
          m_orbitDistance{ other.m_orbitDistance },
          m_cache(std::make_unique<Cache>(*other.m_cache))
    {
        // Controller is not copied
    }

    Camera::Camera(Camera&& other) noexcept
        : Spatial(std::move(other)),
          m_settings(other.m_settings),
          m_target(other.m_target),
          m_orbitDistance{ other.m_orbitDistance },
          m_controller(std::move(other.m_controller)),
          m_cache(std::move(other.m_cache))
    {
    }

    Camera& Camera::operator=(const Camera& other)
    {
        if (this != &other)
        {
            Spatial::operator=(other);
            m_settings = other.m_settings;
            m_target = other.m_target;
            m_orbitDistance = other.m_orbitDistance;
            *m_cache = *other.m_cache;
            // Controller is not copied
        }
        return *this;
    }

    Camera& Camera::operator=(Camera&& other) noexcept
    {
        if (this != &other)
        {
            Spatial::operator=(std::move(other));
            m_settings = other.m_settings;
            m_target = other.m_target;
            m_orbitDistance = other.m_orbitDistance;
            m_cache = std::move(other.m_cache);
            m_controller = std::move(other.m_controller);
        }
        return *this;
    }

    Camera Camera::createLookAt(const glm::vec3& position, const glm::vec3& target, const glm::vec3& up)
    {
        Camera camera{};

        // Calculate forward direction
        const auto forward{ glm::normalize(target - position) };

        // Calculate yaw and pitch from forward vector
        const auto yaw{ glm::degrees(std::atan2(forward.z, forward.x)) };
        const auto pitch{ glm::degrees(std::asin(forward.y)) };

        // Set camera properties
        camera.setPosition(position);
        camera.setWorldUp(up);
        camera.setOrientation(yaw, pitch);

        return camera;
    }

    const glm::mat4& Camera::getViewMatrix() const
    {
        if (m_cache->viewDirty)
        {
            if (m_settings.mode == CameraMode::Orbital && m_target.has_value())
            {
                // For orbital camera, position is calculated based on target and distance
                const auto calculatedPosition{ m_target.value() - getForward() * m_orbitDistance };
                m_cache->viewMatrix = glm::lookAt(calculatedPosition, m_target.value(), getUp());
            }
            else
            {
                m_cache->viewMatrix = glm::lookAt(getPosition(), getPosition() + getForward(), getUp());
            }
            m_cache->viewDirty = false;
        }
        return m_cache->viewMatrix;
    }

    const glm::mat4& Camera::getProjectionMatrix() const
    {
        if (m_cache->projectionDirty)
        {
            if (m_settings.type == CameraType::Perspective)
            {
                m_cache->projectionMatrix = glm::perspective(
                    glm::radians(m_settings.fieldOfView),
                    m_settings.aspectRatio,
                    m_settings.nearPlane,
                    m_settings.farPlane
                );
            }
            else
            {
                // Orthographic
                const float orthoSize{ m_settings.fieldOfView * 0.01f }; // Scale factor
                m_cache->projectionMatrix = glm::ortho(
                    -orthoSize * m_settings.aspectRatio,
                    orthoSize * m_settings.aspectRatio,
                    -orthoSize,
                    orthoSize,
                    m_settings.nearPlane,
                    m_settings.farPlane
                );
            }
            m_cache->projectionDirty = false;
        }
        return m_cache->projectionMatrix;
    }

    void Camera::setTarget(const glm::vec3& target)
    {
        m_target = target;
        m_cache->viewDirty = true;
    }

    void Camera::clearTarget()
    {
        m_target.reset();
        m_cache->viewDirty = true;
    }

    void Camera::setOrbitDistance(const float distance)
    {
        m_orbitDistance = std::max(MIN_ORBIT_DISTANCE, distance);
        m_cache->viewDirty = true;
    }

    void Camera::setFieldOfView(const float fov)
    {
        m_settings.fieldOfView = std::clamp(fov, MIN_FOV, MAX_FOV);
        m_cache->projectionDirty = true;
    }

    void Camera::setAspectRatio(const float aspectRatio)
    {
        m_settings.aspectRatio = aspectRatio;
        m_cache->projectionDirty = true;
    }

    void Camera::setNearPlane(const float nearPlane)
    {
        m_settings.nearPlane = nearPlane;
        m_cache->projectionDirty = true;
    }

    void Camera::setFarPlane(const float farPlane)
    {
        m_settings.farPlane = farPlane;
        m_cache->projectionDirty = true;
    }

    void Camera::setCameraType(const CameraType type)
    {
        m_settings.type = type;
        m_cache->projectionDirty = true;
    }

    void Camera::setCameraMode(const CameraMode mode)
    {
        m_settings.mode = mode;
        m_cache->viewDirty = true;
    }

    const Camera::CameraSettings& Camera::settings() const
    {
        return m_settings;
    }

    std::optional<glm::vec3> Camera::optionalTarget() const
    {
        return m_target;
    }

    Camera::Frustum Camera::extractFrustum() const
    {
        Frustum frustum{};
        auto vp{ getProjectionMatrix() * getViewMatrix() };

        // Right plane
        frustum.planes[Frustum::Right].x = vp[0][3] - vp[0][0];
        frustum.planes[Frustum::Right].y = vp[1][3] - vp[1][0];
        frustum.planes[Frustum::Right].z = vp[2][3] - vp[2][0];
        frustum.planes[Frustum::Right].w = vp[3][3] - vp[3][0];

        // Left plane
        frustum.planes[Frustum::Left].x = vp[0][3] + vp[0][0];
        frustum.planes[Frustum::Left].y = vp[1][3] + vp[1][0];
        frustum.planes[Frustum::Left].z = vp[2][3] + vp[2][0];
        frustum.planes[Frustum::Left].w = vp[3][3] + vp[3][0];

        // Bottom plane
        frustum.planes[Frustum::Bottom].x = vp[0][3] + vp[0][1];
        frustum.planes[Frustum::Bottom].y = vp[1][3] + vp[1][1];
        frustum.planes[Frustum::Bottom].z = vp[2][3] + vp[2][1];
        frustum.planes[Frustum::Bottom].w = vp[3][3] + vp[3][1];

        // Top plane
        frustum.planes[Frustum::Top].x = vp[0][3] - vp[0][1];
        frustum.planes[Frustum::Top].y = vp[1][3] - vp[1][1];
        frustum.planes[Frustum::Top].z = vp[2][3] - vp[2][1];
        frustum.planes[Frustum::Top].w = vp[3][3] - vp[3][1];

        // Far plane
        frustum.planes[Frustum::Far].x = vp[0][3] - vp[0][2];
        frustum.planes[Frustum::Far].y = vp[1][3] - vp[1][2];
        frustum.planes[Frustum::Far].z = vp[2][3] - vp[2][2];
        frustum.planes[Frustum::Far].w = vp[3][3] - vp[3][2];

        // Near plane
        frustum.planes[Frustum::Near].x = vp[0][3] + vp[0][2];
        frustum.planes[Frustum::Near].y = vp[1][3] + vp[1][2];
        frustum.planes[Frustum::Near].z = vp[2][3] + vp[2][2];
        frustum.planes[Frustum::Near].w = vp[3][3] + vp[3][2];

        // Normalize all planes
        for (auto& plane : frustum.planes)
        {
            const float length = std::sqrt(plane.x * plane.x + plane.y * plane.y + plane.z * plane.z);
            plane /= length;
        }

        return frustum;
    }

    bool Camera::isPointVisible(const glm::vec3& point, const Frustum& frustum)
    {
        return std::ranges::all_of(frustum.planes, [&point](const auto& plane)
        {
            return glm::dot(glm::vec3(plane), point) + plane.w >= 0;
        });
    }

    bool Camera::isSphereVisible(const glm::vec3& center, float radius, const Frustum& frustum)
    {
        return std::ranges::all_of(frustum.planes, [&center, radius](const auto& plane)
        {
            return glm::dot(glm::vec3(plane), center) + plane.w >= -radius;
        });
    }

    bool Camera::isAABBVisible(const glm::vec3& min, const glm::vec3& max, const Frustum& frustum)
    {
        for (const auto& plane : frustum.planes)
        {
            glm::vec3 p{ min };
            glm::vec3 n{ max };

            // Select the positive vertex based on plane normal
            if (plane.x >= 0)
            {
                p.x = max.x;
                n.x = min.x;
            }
            if (plane.y >= 0)
            {
                p.y = max.y;
                n.y = min.y;
            }
            if (plane.z >= 0)
            {
                p.z = max.z;
                n.z = min.z;
            }

            // If the positive vertex is outside, the box is outside
            if (glm::dot(glm::vec3{ plane }, p) + plane.w < 0)
            {
                return false;
            }
        }
        return true;
    }

    glm::vec3 Camera::screenToWorld(const glm::vec2& screenPos, const glm::vec2& screenSize) const
    {
        // Convert screen coordinates to normalized device coordinates
        const glm::vec2 ndc{
            (2.0f * screenPos.x) / screenSize.x - 1.0f,
            1.0f - (2.0f * screenPos.y) / screenSize.y
        };

        // Create a ray from the near to far plane
        const glm::vec4 rayClip{ ndc.x, ndc.y, -1.0f, 1.0f };
        auto rayEye{ glm::inverse(getProjectionMatrix()) * rayClip };
        rayEye = glm::vec4{ rayEye.x, rayEye.y, -1.0f, 0.0f };

        auto rayWorld{ glm::vec3{ glm::inverse(getViewMatrix()) * rayEye } };
        rayWorld = glm::normalize(rayWorld);

        return rayWorld;
    }

    Camera::Ray Camera::createRayFromScreen(const glm::vec2& screenPos, const glm::vec2& screenSize) const
    {
        return {
            .origin = getPosition(),
            .direction = screenToWorld(screenPos, screenSize)
        };
    }

    std::string Camera::serialize() const
    {
        // Simple serialization to string - in a real engine, use a proper serialization library
        return std::format(
            "Camera:\n"
            "  Position: {}, {}, {}\n"
            "  Orientation: {}, {}, {}\n"
            "  FOV: {}\n"
            "  Near/Far: {}/{}\n",
            getPosition().x, getPosition().y, getPosition().z,
            getYaw(), getPitch(), getRoll(),
            m_settings.fieldOfView,
            m_settings.nearPlane, m_settings.farPlane
        );
    }

    void Camera::interpolateTo(const Camera& target, float t)
    {
        t = std::clamp(t, 0.0f, 1.0f);

        // Interpolate position
        setPosition(glm::mix(getPosition(), target.getPosition(), t));

        // Interpolate orientation (careful with yaw wrapping)
        auto yawDiff{ target.getYaw() - getYaw() };
        if (yawDiff > 180.0f)
        {
            yawDiff -= 360.0f;
        }
        else if (yawDiff < -180.0f)
        {
            yawDiff += 360.0f;
        }
        setOrientation(
            getYaw() + yawDiff * t,
            glm::mix(getPitch(), target.getPitch(), t),
            glm::mix(getRoll(), target.getRoll(), t)
        );

        // Interpolate settings
        m_settings.fieldOfView = glm::mix(m_settings.fieldOfView, target.m_settings.fieldOfView, t);
        m_settings.nearPlane = glm::mix(m_settings.nearPlane, target.m_settings.nearPlane, t);
        m_settings.farPlane = glm::mix(m_settings.farPlane, target.m_settings.farPlane, t);

        // Mark matrices as dirty
        m_cache->projectionDirty = true;
    }

    FirstPersonController::FirstPersonController(const float initialX, const float initialY)
        : m_lastX{ initialX }, m_lastY{ initialY }
    {
    }

    void FirstPersonController::update(Camera& camera, const float deltaTime)
    {
        const auto velocity{ camera.movementSpeed() * deltaTime };

        if (m_keys[GLFW_KEY_W])
        {
            camera.moveForward(velocity);
        }
        if (m_keys[GLFW_KEY_S])
        {
            camera.moveForward(-velocity);
        }
        if (m_keys[GLFW_KEY_A])
        {
            camera.moveRight(-velocity);
        }
        if (m_keys[GLFW_KEY_D])
        {
            camera.moveRight(velocity);
        }
        if (m_keys[GLFW_KEY_SPACE])
        {
            camera.moveUp(velocity);
        }
        if (m_keys[GLFW_KEY_LEFT_CONTROL])
        {
            camera.moveUp(-velocity);
        }
    }

    void FirstPersonController::processKeyInput(Camera& camera, const int key, const int action)
    {
        if (key >= 0 && key < 1024)
        {
            if (action == GLFW_PRESS)
            {
                m_keys[key] = true;
            }
            else if (action == GLFW_RELEASE)
            {
                m_keys[key] = false;
            }
        }
    }

    void FirstPersonController::processMouseMovement(Camera& camera, const float xPos, const float yPos)
    {
        if (m_firstMouse)
        {
            m_lastX = xPos;
            m_lastY = yPos;
            m_firstMouse = false;
        }

        const auto xOffset{ xPos - m_lastX };
        const auto yOffset{ m_lastY - yPos }; // Reversed: y ranges bottom to top

        m_lastX = xPos;
        m_lastY = yPos;

        camera.rotate(xOffset, yOffset);
    }

    void FirstPersonController::processMouseScroll(Camera& camera, const float yOffset)
    {
        camera.setFieldOfView(camera.settings().fieldOfView - yOffset);
    }

    OrbitalController::OrbitalController(const float initialX, const float initialY)
        : m_lastX{ initialX }, m_lastY{ initialY }
    {
    }

    void OrbitalController::update(Camera& camera, float deltaTime)
    {
        // Nothing to do in update for orbital camera
    }

    void OrbitalController::processKeyInput(Camera& camera, int key, int action)
    {
        // No keyboard controls for orbital camera
    }

    void OrbitalController::processMouseMovement(Camera& camera, const float xPos, const float yPos)
    {
        if (m_rotating)
        {
            const auto xOffset{ xPos - m_lastX };
            const auto yOffset{ m_lastY - yPos };

            camera.rotate(xOffset, yOffset);
        }

        m_lastX = xPos;
        m_lastY = yPos;
    }

    void OrbitalController::processMouseScroll(Camera& camera, const float yOffset)
    {
        // Adjust orbit distance
        const auto currentDistance{
            glm::length(camera.getPosition() - camera.optionalTarget().value_or(glm::vec3{ 0.0f }))
        };
        const auto newDistance{ currentDistance - yOffset };
        camera.setOrbitDistance(newDistance);
    }

    void OrbitalController::setRotating(const bool rotating)
    {
        m_rotating = rotating;
    }

    void CameraSystem::keyCallback(GLFWwindow* window, const int key, int scancode, const int action, int mods)
    {
        const auto system{ static_cast<CameraSystem*>(glfwGetWindowUserPointer(window)) };
        system->m_controller->processKeyInput(system->m_camera, key, action);
    }

    void CameraSystem::mouseCallback(GLFWwindow* window, const double xPos, const double yPos)
    {
        const auto system{ static_cast<CameraSystem*>(glfwGetWindowUserPointer(window)) };
        system->m_controller->processMouseMovement(system->m_camera,
                                                   static_cast<float>(xPos),
                                                   static_cast<float>(yPos));
    }

    void CameraSystem::scrollCallback(GLFWwindow* window, double xOffset, const double yOffset)
    {
        const auto system{ static_cast<CameraSystem*>(glfwGetWindowUserPointer(window)) };
        system->m_controller->processMouseScroll(system->m_camera, static_cast<float>(yOffset));
    }

    CameraSystem::CameraSystem(GLFWwindow* window, const Camera::CameraSettings& settings)
        : m_camera{ settings }, m_window{ window }
    {
        // Set up controller based on camera mode
        if (settings.mode == CameraMode::FirstPerson)
        {
            m_controller = std::make_unique<FirstPersonController>();
        }
        else if (settings.mode == CameraMode::Orbital)
        {
            m_controller = std::make_unique<OrbitalController>();
            m_camera.setTarget(glm::vec3{ 0.0f });
        }
        else
        {
            std::println(stderr, "Unsupported camera mode is using.");
        }

        // Set up GLFW callbacks
        glfwSetWindowUserPointer(window, this);
        glfwSetKeyCallback(window, keyCallback);
        glfwSetCursorPosCallback(window, mouseCallback);
        glfwSetScrollCallback(window, scrollCallback);
    }

    void CameraSystem::update(const float deltaTime)
    {
        m_controller->update(m_camera, deltaTime);
    }

    Camera& CameraSystem::getCamera()
    {
        return m_camera;
    }

    const Camera& CameraSystem::getCamera() const
    {
        return m_camera;
    }
} // lgl
