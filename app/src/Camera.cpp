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
    const Camera::CameraSettings Camera::DEFAULT_CAMERA_SETTINGS{};

    void Camera::updateCameraVectors()
    {
        // Calculate the new forward vector
        m_forward.x = std::cos(glm::radians(m_yaw)) * std::cos(glm::radians(m_pitch));
        m_forward.y = std::sin(glm::radians(m_pitch));
        m_forward.z = std::sin(glm::radians(m_yaw)) * std::cos(glm::radians(m_pitch));
        m_forward = glm::normalize(m_forward);

        // Recalculate the right and up vector
        m_right = glm::normalize(glm::cross(m_forward, m_worldUp));
        m_up = glm::normalize(glm::cross(m_right, m_forward));

        m_viewDirty = true;
    }

    Camera::Camera(const CameraSettings& settings)
        : m_settings{ settings }
    {
        updateCameraVectors();
    }

    Camera Camera::createLookAt(const glm::vec3& position, const glm::vec3& target, const glm::vec3& up)
    {
        Camera camera{};
        camera.m_position = position;
        camera.m_worldUp = up;

        // Calculate forward direction
        camera.m_forward = glm::normalize(target - position);

        // Calculate yaw and pitch from forward vector
        camera.m_yaw = glm::degrees(std::atan2(camera.m_forward.z, camera.m_forward.x));
        camera.m_pitch = glm::degrees(std::asin(camera.m_forward.y));

        camera.updateCameraVectors();
        return camera;
    }

    const glm::mat4& Camera::getViewMatrix() const
    {
        if (m_viewDirty)
        {
            if (m_settings.mode == CameraMode::Orbital && m_target.has_value())
            {
                // For orbital camera, position is calculated based on target and distance
                m_position = m_target.value() - m_forward * m_orbitDistance;
                m_viewMatrix = glm::lookAt(m_position, m_target.value(), m_up);
            }
            else
            {
                m_viewMatrix = glm::lookAt(m_position, m_position + m_forward, m_up);
            }
            m_viewDirty = false;
        }
        return m_viewMatrix;
    }

    const glm::mat4& Camera::getProjectionMatrix() const
    {
        if (m_projectionDirty)
        {
            if (m_settings.type == CameraType::Perspective)
            {
                m_projectionMatrix = glm::perspective(
                    glm::radians(m_settings.fieldOfView),
                    m_settings.aspectRatio,
                    m_settings.nearPlane,
                    m_settings.farPlane
                );
            }
            else
            {
                // Orthographic
                const float orthoSize = m_settings.fieldOfView * 0.01f; // Scale factor
                m_projectionMatrix = glm::ortho(
                    -orthoSize * m_settings.aspectRatio,
                    orthoSize * m_settings.aspectRatio,
                    -orthoSize,
                    orthoSize,
                    m_settings.nearPlane,
                    m_settings.farPlane
                );
            }
            m_projectionDirty = false;
        }
        return m_projectionMatrix;
    }

    void Camera::moveForward(const float distance)
    {
        m_position += m_forward * distance * m_settings.movementSpeed;
        m_viewDirty = true;
    }

    void Camera::moveRight(const float distance)
    {
        m_position += m_right * distance * m_settings.movementSpeed;
        m_viewDirty = true;
    }

    void Camera::moveUp(const float distance)
    {
        m_position += m_worldUp * distance * m_settings.movementSpeed;
        m_viewDirty = true;
    }

    void Camera::rotate(const float yawOffset, const float pitchOffset)
    {
        m_yaw += yawOffset * m_settings.mouseSensitivity;
        m_pitch += pitchOffset * m_settings.mouseSensitivity;

        // Constrain pitch to avoid gimbal lock
        m_pitch = std::clamp(m_pitch, -89.0f, 89.0f);

        // Update vectors
        updateCameraVectors();
    }

    void Camera::setRoll(const float roll)
    {
        m_roll = roll;
        updateCameraVectors();
    }

    void Camera::setTarget(const glm::vec3& target)
    {
        m_target = target;
        m_viewDirty = true;
    }

    void Camera::clearTarget()
    {
        m_target.reset();
        m_viewDirty = true;
    }

    void Camera::setOrbitDistance(const float distance)
    {
        m_orbitDistance = std::max(0.1f, distance);
        m_viewDirty = true;
    }

    void Camera::setFieldOfView(const float fov)
    {
        m_settings.fieldOfView = std::clamp(fov, 1.0f, 170.0f);
        m_projectionDirty = true;
    }

    void Camera::setAspectRatio(const float aspectRatio)
    {
        m_settings.aspectRatio = aspectRatio;
        m_projectionDirty = true;
    }

    void Camera::setNearPlane(const float nearPlane)
    {
        m_settings.nearPlane = nearPlane;
        m_projectionDirty = true;
    }

    void Camera::setFarPlane(const float farPlane)
    {
        m_settings.farPlane = farPlane;
        m_projectionDirty = true;
    }

    void Camera::setCameraType(const CameraType type)
    {
        m_settings.type = type;
        m_projectionDirty = true;
    }

    void Camera::setCameraMode(const CameraMode mode)
    {
        m_settings.mode = mode;
        m_viewDirty = true;
    }

    const glm::vec3& Camera::getPosition() const
    {
        return m_position;
    }

    const glm::vec3& Camera::getForward() const
    {
        return m_forward;
    }

    const glm::vec3& Camera::getUp() const
    {
        return m_up;
    }

    const glm::vec3& Camera::getRight() const
    {
        return m_right;
    }

    float Camera::getYaw() const
    {
        return m_yaw;
    }

    float Camera::getPitch() const
    {
        return m_pitch;
    }

    float Camera::getRoll() const
    {
        return m_roll;
    }

    void Camera::setPosition(const glm::vec3& position)
    {
        m_position = position;
        m_viewDirty = true;
    }

    void Camera::setOrientation(const float yaw, const float pitch)
    {
        m_yaw = yaw;
        m_pitch = std::clamp(pitch, -89.0f, 89.0f);
        updateCameraVectors();
    }

    void Camera::setWorldUp(const glm::vec3& up)
    {
        m_worldUp = glm::normalize(up);
        updateCameraVectors();
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
        glm::mat4 vp = getProjectionMatrix() * getViewMatrix();

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
            .origin = m_position,
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
            m_position.x, m_position.y, m_position.z,
            m_yaw, m_pitch, m_roll,
            m_settings.fieldOfView,
            m_settings.nearPlane, m_settings.farPlane
        );
    }

    void Camera::interpolateTo(const Camera& target, float t)
    {
        t = std::clamp(t, 0.0f, 1.0f);

        // Interpolate position
        m_position = glm::mix(m_position, target.m_position, t);

        // Interpolate orientation (careful with yaw wrapping)
        auto yawDiff{ target.m_yaw - m_yaw };
        if (yawDiff > 180.0f)
        {
            yawDiff -= 360.0f;
        }
        else if (yawDiff < -180.0f)
        {
            yawDiff += 360.0f;
        }

        m_yaw += yawDiff * t;
        m_pitch = glm::mix(m_pitch, target.m_pitch, t);
        m_roll = glm::mix(m_roll, target.m_roll, t);

        // Interpolate settings
        m_settings.fieldOfView = glm::mix(m_settings.fieldOfView, target.m_settings.fieldOfView, t);
        m_settings.nearPlane = glm::mix(m_settings.nearPlane, target.m_settings.nearPlane, t);
        m_settings.farPlane = glm::mix(m_settings.farPlane, target.m_settings.farPlane, t);

        // Update vectors and mark matrices as dirty
        updateCameraVectors();
        m_projectionDirty = true;
    }

    FirstPersonController::FirstPersonController(const float initialX, const float initialY)
        : m_lastX{ initialX }, m_lastY{ initialY }
    {
    }

    void FirstPersonController::update(Camera& camera, const float deltaTime)
    {
        const float velocity = camera.settings().movementSpeed * deltaTime;

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
