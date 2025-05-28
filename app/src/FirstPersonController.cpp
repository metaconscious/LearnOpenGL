//
// Created by user on 5/28/25.
//

#include "app/FirstPersonController.h"

namespace lgl
{
    void FirstPersonController::setCamera(const std::shared_ptr<Camera> camera)
    {
        m_camera = camera;

        // Initialize default key bindings
        setupDefaultBindings();
    }

    void FirstPersonController::update(const float deltaTime)
    {
        if (!m_camera)
        {
            return;
        }

        // Process all active key bindings
        for (const auto& [key, isPressed] : m_keyStates)
        {
            if (isPressed && m_keyBindings.contains(key))
            {
                m_keyBindings[key](deltaTime);
            }
        }
    }

    void FirstPersonController::processKeyInput(const int key,
                                                [[maybe_unused]] const int scancode,
                                                const int action,
                                                const int mods)
    {
        // Update key state
        if (action == GLFW_PRESS)
        {
            m_keyStates[key] = true;
        }
        else if (action == GLFW_RELEASE)
        {
            m_keyStates[key] = false;
        }

        // Handle cursor toggle
        handleCursorToggle(key, action, mods);
    }

    void FirstPersonController::processMouseMovement(const double xPos, const double yPos)
    {
        if (m_cursorEnabled)
        {
            return;
        }

        if (m_firstMouse)
        {
            m_lastMousePos.x = xPos;
            m_lastMousePos.y = yPos;
            m_firstMouse = false;
            return;
        }

        // Calculate offset
        float xOffset = static_cast<float>(m_lastMousePos.x - xPos);
        float yOffset = static_cast<float>(m_lastMousePos.y - yPos);

        m_lastMousePos.x = xPos;
        m_lastMousePos.y = yPos;

        // Apply sensitivity
        xOffset *= m_mouseSensitivity;
        yOffset *= m_mouseSensitivity;

        // Update camera orientation
        m_yaw += xOffset;
        m_pitch += yOffset;

        // Constrain pitch to avoid gimbal lock
        m_pitch = glm::clamp(m_pitch, -89.0f, 89.0f);

        // Update camera rotation using quaternion
        updateCameraRotation();
    }

    void FirstPersonController::processMouseButton(int button, int action, int mods)
    {
        // Can be extended for mouse button actions
    }

    void FirstPersonController::processMouseScroll([[maybe_unused]] const double xOffset,
                                                   const double yOffset)
    {
        // Can be used to implement zoom by changing FOV
        // For a perspective camera implementation:
        if (const auto perspectiveCamera{ std::dynamic_pointer_cast<PerspectiveCamera>(m_camera) })
        {
            const float fov{ glm::clamp(perspectiveCamera->getFov() - static_cast<float>(yOffset), 1.0f, 90.0f) };
            perspectiveCamera->setFov(fov);
        }
    }

    bool FirstPersonController::handleCursorToggle(const int key,
                                                   const int action,
                                                   [[maybe_unused]] const int mods)
    {
        if (key == m_cursorToggleKey && action == GLFW_PRESS)
        {
            m_cursorEnabled = !m_cursorEnabled;
            return true;
        }
        return false;
    }

    void FirstPersonController::bindKey(const int key, const KeyActionCallback action)
    {
        m_keyBindings[key] = action;
    }

    void FirstPersonController::setMoveSpeed(const float speed)
    {
        m_moveSpeed = speed;
    }

    float FirstPersonController::getMoveSpeed() const
    {
        return m_moveSpeed;
    }

    void FirstPersonController::setMouseSensitivity(const float sensitivity)
    {
        m_mouseSensitivity = sensitivity;
    }

    float FirstPersonController::getMouseSensitivity() const
    {
        return m_mouseSensitivity;
    }

    void FirstPersonController::setKeyMapping(const int forwardKey,
                                              const int backwardKey,
                                              const int leftKey,
                                              const int rightKey,
                                              const int upKey,
                                              const int downKey,
                                              const int cursorToggleKey)
    {
        m_forwardKey = forwardKey;
        m_backwardKey = backwardKey;
        m_leftKey = leftKey;
        m_rightKey = rightKey;
        m_upKey = upKey;
        m_downKey = downKey;
        m_cursorToggleKey = cursorToggleKey;

        // Reset bindings and set up new ones
        m_keyBindings.clear();
        setupDefaultBindings();
    }

    bool FirstPersonController::isCursorEnabled() const
    {
        return m_cursorEnabled;
    }

    void FirstPersonController::enableCursor(const bool enable)
    {
        m_cursorEnabled = enable;
    }

    void FirstPersonController::updateCameraRotation() const
    {
        if (!m_camera)
        {
            return;
        }

        m_camera->setEulerAngles({ m_pitch, m_yaw, 0.0f });
    }

    void FirstPersonController::setupDefaultBindings()
    {
        // Forward/backward movement
        bindKey(m_forwardKey,
                [this](const float dt)
                {
                    if (!m_camera)
                    {
                        return;
                    }
                    m_camera->setPosition(m_camera->getPosition() + m_camera->getForwardVector() * m_moveSpeed * dt);
                }
        );

        bindKey(m_backwardKey,
                [this](const float dt)
                {
                    if (!m_camera)
                    {
                        return;
                    }
                    m_camera->setPosition(m_camera->getPosition() - m_camera->getForwardVector() * m_moveSpeed * dt);
                }
        );

        // Left/right movement
        bindKey(m_leftKey,
                [this](const float dt)
                {
                    if (!m_camera)
                    {
                        return;
                    }
                    m_camera->setPosition(m_camera->getPosition() - m_camera->getRightVector() * m_moveSpeed * dt);
                }
        );

        bindKey(m_rightKey,
                [this](const float dt)
                {
                    if (!m_camera)
                    {
                        return;
                    }
                    m_camera->setPosition(m_camera->getPosition() + m_camera->getRightVector() * m_moveSpeed * dt);
                }
        );

        // Up/down movement
        bindKey(m_upKey,
                [this](const float dt)
                {
                    if (!m_camera)
                    {
                        return;
                    }
                    m_camera->setPosition(m_camera->getPosition() + glm::vec3(0.0f, 1.0f, 0.0f) * m_moveSpeed * dt);
                }
        );

        bindKey(m_downKey,
                [this](const float dt)
                {
                    if (!m_camera)
                    {
                        return;
                    }
                    m_camera->setPosition(m_camera->getPosition() - glm::vec3(0.0f, 1.0f, 0.0f) * m_moveSpeed * dt);
                }
        );
    }
} // lgl
