//
// Created by user on 5/23/25.
//

#include "app/Camera.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

#include <algorithm>
#include <format>
#include <print>

namespace lgl
{
    const Camera::CameraSettings Camera::DEFAULT_CAMERA_SETTINGS{};

    void Camera::onSpatialChanged()
    {
        m_cache->viewDirty = true;
    }

    Camera::Camera(const CameraSettings& settings)
        : Spatial{ constants::DEFAULT_POSITION, constants::DEFAULT_YAW, constants::DEFAULT_PITCH },
          m_settings{ settings },
          m_cache{ std::make_unique<Cache>() }
    {
    }

    Camera::Camera(const Camera& other)
        : Spatial{ other },
          m_settings{ other.m_settings },
          m_target{ other.m_target },
          m_orbitDistance{ other.m_orbitDistance },
          m_cache{ std::make_unique<Cache>(*other.m_cache) }
    {
        // Controller is not copied
    }

    Camera::Camera(Camera&& other) noexcept
        : Spatial{ std::move(other) },
          m_settings{ other.m_settings },
          m_target{ other.m_target },
          m_orbitDistance{ other.m_orbitDistance },
          m_controller{ std::move(other.m_controller) },
          m_cache{ std::move(other.m_cache) }
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
        camera.setPosition(position);
        camera.getTransform().lookAt(target, up);
        camera.updateEulerAngles();
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
        m_orbitDistance = std::max(constants::MIN_ORBIT_DISTANCE, distance);
        m_cache->viewDirty = true;
    }

    void Camera::setFieldOfView(const float fov)
    {
        m_settings.fieldOfView = std::clamp(fov, constants::MIN_FOV, constants::MAX_FOV);
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

    void Camera::setMouseSensitivity(const float sensitivity)
    {
        m_settings.mouseSensitivity = sensitivity;
    }

    const Camera::CameraSettings& Camera::settings() const
    {
        return m_settings;
    }

    std::optional<glm::vec3> Camera::optionalTarget() const
    {
        return m_target;
    }

    float Camera::getMouseSensitivity() const
    {
        return m_settings.mouseSensitivity;
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
            if (const auto lengthSquared{ plane.x * plane.x + plane.y * plane.y + plane.z * plane.z };
                lengthSquared > 0.0001f)
            {
                // Avoid division by very small numbers
                const float invLength = 1.0f / std::sqrt(lengthSquared);
                plane *= invLength;
            }
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

        // Use the base class interpolation for position and orientation
        Spatial::interpolateTo(target, t);

        // Interpolate camera-specific settings
        m_settings.fieldOfView = glm::mix(m_settings.fieldOfView, target.m_settings.fieldOfView, t);
        m_settings.nearPlane = glm::mix(m_settings.nearPlane, target.m_settings.nearPlane, t);
        m_settings.farPlane = glm::mix(m_settings.farPlane, target.m_settings.farPlane, t);

        // Mark matrices as dirty
        m_cache->projectionDirty = true;
    }

    // CameraController implementation
    void CameraController::setInputBindings(const std::vector<InputBinding>& bindings)
    {
        m_inputBindings = bindings;
        rebuildBindingMaps();
    }

    const std::vector<InputBinding>& CameraController::getInputBindings() const
    {
        return m_inputBindings;
    }

    void CameraController::rebuildBindingMaps()
    {
        // Clear existing maps
        m_keyBindings.clear();
        m_mouseButtonBindings.clear();
        m_mouseMovementBinding.reset();
        m_scrollWheelBinding.reset();

        // Rebuild maps from input bindings
        for (const auto& [key, mouseButton, isMouseMovement, isScrollWheel, action] : m_inputBindings)
        {
            if (isMouseMovement)
            {
                m_mouseMovementBinding = action;
            }
            else if (isScrollWheel)
            {
                m_scrollWheelBinding = action;
            }
            else if (mouseButton >= 0)
            {
                m_mouseButtonBindings[mouseButton] = action;
            }
            else if (key != GLFW_KEY_UNKNOWN)
            {
                m_keyBindings[key] = action;
            }
        }
    }

    // FirstPersonController implementation
    FirstPersonController::FirstPersonController(const float initialX, const float initialY)
        : m_lastX{ initialX }, m_lastY{ initialY }
    {
        setDefaultBindings();
    }

    void FirstPersonController::setDefaultBindings()
    {
        const std::vector<InputBinding> bindings{
            { GLFW_KEY_W, -1, false, false, CameraAction::MoveForward },
            { GLFW_KEY_S, -1, false, false, CameraAction::MoveBackward },
            { GLFW_KEY_A, -1, false, false, CameraAction::MoveLeft },
            { GLFW_KEY_D, -1, false, false, CameraAction::MoveRight },
            { GLFW_KEY_SPACE, -1, false, false, CameraAction::MoveUp },
            { GLFW_KEY_LEFT_CONTROL, -1, false, false, CameraAction::MoveDown },
            { GLFW_KEY_UNKNOWN, -1, true, false, CameraAction::RotateCamera },
            { GLFW_KEY_UNKNOWN, -1, false, true, CameraAction::ZoomIn } // Scroll wheel handles both zoom in/out
        };

        setInputBindings(bindings);
    }

    void FirstPersonController::update(Camera& camera, const float deltaTime)
    {
        const auto velocity{ camera.movementSpeed() * deltaTime };

        if (m_actionStates[static_cast<size_t>(CameraAction::MoveForward)])
        {
            camera.moveForward(velocity);
        }
        if (m_actionStates[static_cast<size_t>(CameraAction::MoveBackward)])
        {
            camera.moveForward(-velocity);
        }
        if (m_actionStates[static_cast<size_t>(CameraAction::MoveLeft)])
        {
            camera.moveRight(-velocity);
        }
        if (m_actionStates[static_cast<size_t>(CameraAction::MoveRight)])
        {
            camera.moveRight(velocity);
        }
        if (m_actionStates[static_cast<size_t>(CameraAction::MoveUp)])
        {
            camera.moveUp(velocity);
        }
        if (m_actionStates[static_cast<size_t>(CameraAction::MoveDown)])
        {
            camera.moveUp(-velocity);
        }
    }

    void FirstPersonController::processKeyInput(Camera& camera, const int key, const int action)
    {
        // Update key state
        if (key >= 0 && key < 1024)
        {
            m_keys[key] = (action == GLFW_PRESS || action == GLFW_REPEAT);
        }

        // Check if this key maps to a camera action
        if (const auto it{ m_keyBindings.find(key) }; it != m_keyBindings.end())
        {
            m_actionStates[static_cast<size_t>(it->second)] = (action == GLFW_PRESS || action == GLFW_REPEAT);
        }
    }

    void FirstPersonController::processMouseMovement(Camera& camera, const float xPos, const float yPos)
    {
        if (m_firstMouse)
        {
            m_lastX = xPos;
            m_lastY = yPos;
            m_firstMouse = false;
            return;
        }

        // Only process mouse movement if we have a binding for it
        if (!m_mouseMovementBinding.has_value())
        {
            m_lastX = xPos;
            m_lastY = yPos;
            return;
        }

        const auto xOffset{ (m_lastX - xPos) * camera.getMouseSensitivity() };
        const auto yOffset{ (m_lastY - yPos) * camera.getMouseSensitivity() };

        m_lastX = xPos;
        m_lastY = yPos;

        camera.rotate(xOffset, yOffset);
    }

    void FirstPersonController::processMouseScroll(Camera& camera, const float yOffset)
    {
        // Only process scroll if we have a binding for it
        if (!m_scrollWheelBinding.has_value())
        {
            return;
        }

        // Adjust field of view (zoom)
        camera.setFieldOfView(camera.settings().fieldOfView - yOffset * 2.0f);
    }

    void FirstPersonController::processMouseButton(Camera& camera, const int button, const int action)
    {
        // Check if this button maps to a camera action
        if (const auto it{ m_mouseButtonBindings.find(button) }; it != m_mouseButtonBindings.end())
        {
            m_actionStates[static_cast<size_t>(it->second)] = (action == GLFW_PRESS);
        }
    }

    // OrbitalController implementation
    OrbitalController::OrbitalController(const float initialX, const float initialY)
        : m_lastX{ initialX }, m_lastY{ initialY }
    {
        setDefaultBindings();
    }

    void OrbitalController::setDefaultBindings()
    {
        const std::vector<InputBinding> bindings{
            { GLFW_KEY_UNKNOWN, GLFW_MOUSE_BUTTON_LEFT, false, false, CameraAction::RotateCamera },
            { GLFW_KEY_UNKNOWN, -1, false, true, CameraAction::ZoomIn } // Scroll wheel handles both zoom in/out
        };

        setInputBindings(bindings);
    }

    void OrbitalController::update(Camera& camera, float deltaTime)
    {
        // Nothing to do in update for orbital camera
    }

    void OrbitalController::processKeyInput(Camera& camera, const int key, const int action)
    {
        // Check if this key maps to a camera action
        if (const auto it{ m_keyBindings.find(key) }; it != m_keyBindings.end())
        {
            m_actionStates[static_cast<size_t>(it->second)] = (action == GLFW_PRESS || action == GLFW_REPEAT);
        }
    }

    void OrbitalController::processMouseMovement(Camera& camera, const float xPos, const float yPos)
    {
        // Only rotate if the left mouse button is pressed (or another configured button)
        if (m_actionStates[static_cast<size_t>(CameraAction::RotateCamera)])
        {
            const auto xOffset{ (m_lastX - xPos) * camera.getMouseSensitivity() };
            const auto yOffset{ (m_lastY - yPos) * camera.getMouseSensitivity() };

            camera.rotate(xOffset, yOffset);
        }

        m_lastX = xPos;
        m_lastY = yPos;
    }

    void OrbitalController::processMouseScroll(Camera& camera, const float yOffset)
    {
        // Only process scroll if we have a binding for it
        if (!m_scrollWheelBinding.has_value())
        {
            return;
        }

        // Adjust orbit distance
        const auto currentDistance{
            camera.optionalTarget().has_value()
            ? glm::length(camera.getPosition() - camera.optionalTarget().value())
            : constants::DEFAULT_ORBIT_DISTANCE
        };
        const auto newDistance{ currentDistance - yOffset };
        camera.setOrbitDistance(newDistance);
    }

    void OrbitalController::processMouseButton(Camera& camera, const int button, const int action)
    {
        // Check if this button maps to a camera action
        if (const auto it{ m_mouseButtonBindings.find(button) }; it != m_mouseButtonBindings.end())
        {
            m_actionStates[static_cast<size_t>(it->second)] = (action == GLFW_PRESS);
        }
    }

    void OrbitalController::setRotating(const bool rotating)
    {
        m_actionStates[static_cast<size_t>(CameraAction::RotateCamera)] = rotating;
    }

    // CameraSystem implementation
    void CameraSystem::keyCallback(GLFWwindow* window,
                                   const int key,
                                   [[maybe_unused]] int scancode,
                                   const int action,
                                   [[maybe_unused]] int mods)
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

    void CameraSystem::mouseButtonCallback(GLFWwindow* window, const int button, const int action, int mods)
    {
        const auto system{ static_cast<CameraSystem*>(glfwGetWindowUserPointer(window)) };
        system->m_controller->processMouseButton(system->m_camera, button, action);
    }

    CameraSystem::CameraSystem(GLFWwindow* window, const Camera::CameraSettings& settings)
        : m_camera{ settings },
          m_window{ window }
    {
        // Set up controller based on camera mode
        switch (settings.mode)
        {
            case CameraMode::Orbital:
                m_controller = std::make_unique<OrbitalController>();
                m_camera.setTarget(glm::vec3{ 0.0f });
                break;
            case CameraMode::FirstPerson:
                [[fallthrough]];
            default:
                if (settings.mode != CameraMode::FirstPerson)
                {
                    std::println(stderr, "Unsupported camera mode is using. Defaulting to FirstPerson.");
                }
                m_controller = std::make_unique<FirstPersonController>();
        }

        // Set up GLFW callbacks
        glfwSetWindowUserPointer(window, this);
        glfwSetKeyCallback(window, keyCallback);
        glfwSetCursorPosCallback(window, mouseCallback);
        glfwSetScrollCallback(window, scrollCallback);
        glfwSetMouseButtonCallback(window, mouseButtonCallback);
    }

    void CameraSystem::update()
    {
        // Calculate delta time
        const auto currentTime{ static_cast<float>(glfwGetTime()) };
        const auto deltaTime{ currentTime - m_lastFrameTime };
        m_lastFrameTime = currentTime;

        // Update camera controller
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

    void CameraSystem::setInputBindings(const std::vector<InputBinding>& bindings) const
    {
        if (m_controller)
        {
            m_controller->setInputBindings(bindings);
        }
    }
} // lgl
