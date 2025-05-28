//
// Created by user on 5/28/25.
//

#include "app/CameraSystem.h"

#include <GLFW/glfw3.h>
#include "app/FirstPersonController.h"
#include "app/PerspectiveCamera.h"

namespace lgl
{
    CameraSystem::CameraSystem(InputManager& inputManager, WindowManager& windowManager)
        : m_inputManager{ inputManager },
          m_windowManager{ windowManager }
    {
        // Create default perspective camera
        m_camera = std::make_shared<PerspectiveCamera>();

        // Set initial aspect ratio from window
        if (const auto perspectiveCamera{ std::dynamic_pointer_cast<PerspectiveCamera>(m_camera) };
            perspectiveCamera != nullptr)
        {
            perspectiveCamera->setAspectRatio(m_windowManager.get().getAspectRatio());
        }

        // Create default controller
        m_controller = std::make_shared<FirstPersonController>();
        m_controller->setCamera(m_camera);

        // Set up input callbacks
        setupCallbacks();

        // Initial cursor state
        m_inputManager.get().setCursorMode(GLFW_CURSOR_DISABLED);
    }

    std::shared_ptr<Camera> CameraSystem::getCamera()
    {
        return m_camera;
    }

    std::shared_ptr<CameraController> CameraSystem::getController()
    {
        return m_controller;
    }

    void CameraSystem::setCamera(const std::shared_ptr<Camera>& camera)
    {
        m_camera = camera;
        if (m_controller)
        {
            m_controller->setCamera(camera);
        }
    }

    void CameraSystem::setController(const std::shared_ptr<CameraController>& controller)
    {
        m_controller = controller;
        if (m_controller && m_camera)
        {
            m_controller->setCamera(m_camera);
        }
    }

    void CameraSystem::update(const float deltaTime) const
    {
        // Update controller with current delta time
        if (m_controller)
        {
            m_controller->update(deltaTime);
        }

        // Update cursor state based on controller
        if (const auto fpController{ std::dynamic_pointer_cast<FirstPersonController>(m_controller) };
            fpController != nullptr)
        {
            m_inputManager.get().setCursorMode(
                fpController->isCursorEnabled()
                ? GLFW_CURSOR_NORMAL
                : GLFW_CURSOR_DISABLED
            );
        }
    }

    void CameraSystem::setupCallbacks() const
    {
        // Register for window resize events
        m_windowManager.get().registerResizeCallback(
            [this](const int width, const int height)
            {
                // Update camera aspect ratio
                if (const auto perspectiveCamera{ std::dynamic_pointer_cast<PerspectiveCamera>(m_camera) };
                    perspectiveCamera != nullptr)
                {
                    const float aspectRatio = width > 0 && height > 0
                                              ? static_cast<float>(width) / static_cast<float>(height)
                                              : 1.0f;
                    perspectiveCamera->setAspectRatio(aspectRatio);
                }
            }
        );

        // Register input callbacks
        m_inputManager.get().registerKeyCallback(
            [this](const int key, const int scancode, const int action, const int mods)
            {
                if (m_controller)
                {
                    m_controller->processKeyInput(key, scancode, action, mods);
                }
            }
        );

        m_inputManager.get().registerMouseMoveCallback(
            [this](const double xPos, const double yPos)
            {
                if (m_controller)
                {
                    m_controller->processMouseMovement(xPos, yPos);
                }
            }
        );

        m_inputManager.get().registerMouseButtonCallback(
            [this](const int button, const int action, const int mods)
            {
                if (m_controller)
                {
                    m_controller->processMouseButton(button, action, mods);
                }
            }
        );

        m_inputManager.get().registerScrollCallback(
            [this](const double xOffset, const double yOffset)
            {
                if (m_controller)
                {
                    m_controller->processMouseScroll(xOffset, yOffset);
                }
            }
        );
    }
} // namespace lgl
