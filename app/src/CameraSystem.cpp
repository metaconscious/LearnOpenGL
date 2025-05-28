//
// Created by user on 5/28/25.
//

#include "app/CameraSystem.h"

namespace lgl
{
    CameraSystem::CameraSystem(GLFWwindow* window)
        : m_window{ window },
          m_lastFrameTime{ std::chrono::high_resolution_clock::now() }
    {
        // Create default perspective camera
        m_camera = std::make_shared<PerspectiveCamera>();

        // Create default controller
        m_controller = std::make_shared<FirstPersonController>();
        m_controller->setCamera(m_camera);

        // Store instance for callbacks
        s_instances.insert_or_assign(window, this);

        // Set up GLFW callbacks
        setupCallbacks();

        // Configure initial cursor state
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

        // Enable raw mouse motion if supported
        if (glfwRawMouseMotionSupported())
        {
            glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
        }
    }

    CameraSystem::~CameraSystem()
    {
        s_instances.erase(m_window);
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

    void CameraSystem::update()
    {
        // Calculate delta time
        const auto currentTime{ std::chrono::high_resolution_clock::now() };
        m_deltaTime = std::chrono::duration<float>{ currentTime - m_lastFrameTime }.count();
        m_lastFrameTime = currentTime;

        // Update controller with current delta time
        if (m_controller)
        {
            m_controller->update(m_deltaTime);
        }

        // Update cursor state based on controller
        if (const auto fpController{ std::dynamic_pointer_cast<FirstPersonController>(m_controller) };
            fpController != nullptr)
        {
            glfwSetInputMode(m_window,
                             GLFW_CURSOR,
                             fpController->isCursorEnabled()
                             ? GLFW_CURSOR_NORMAL
                             : GLFW_CURSOR_DISABLED);
        }
    }

    void CameraSystem::setupCallbacks() const
    {
        glfwSetFramebufferSizeCallback(m_window, framebufferSizeCallback);
        glfwSetKeyCallback(m_window, keyCallback);
        glfwSetCursorPosCallback(m_window, mouseCallback);
        glfwSetMouseButtonCallback(m_window, mouseButtonCallback);
        glfwSetScrollCallback(m_window, scrollCallback);
    }

    void CameraSystem::framebufferSizeCallback(GLFWwindow* window, const int width, const int height)
    {
        if (s_instances.contains(window))
        {
            const auto instance{ s_instances.at(window) };
            // Update camera aspect ratio
            if (const auto perspectiveCamera{ std::dynamic_pointer_cast<PerspectiveCamera>(instance->m_camera) };
                perspectiveCamera != nullptr)
            {
                const float aspectRatio{
                    width > 0 && height > 0
                    ? static_cast<float>(width) / static_cast<float>(height)
                    : 1.0f
                };
                perspectiveCamera->setAspectRatio(aspectRatio);
            }

            // Update viewport
            glViewport(0, 0, width, height);
        }
    }

    void CameraSystem::keyCallback(GLFWwindow* window,
                                   const int key,
                                   const int scancode,
                                   const int action,
                                   const int mods)
    {
        if (s_instances.contains(window))
        {
            if (const auto instance{ s_instances.at(window) };
                instance->m_controller != nullptr)
            {
                instance->m_controller->processKeyInput(key, scancode, action, mods);
            }
        }
    }

    auto CameraSystem::mouseCallback(GLFWwindow* window, const double xPos, const double yPos) -> void
    {
        if (s_instances.contains(window))
        {
            if (const auto instance{ s_instances.at(window) };
                instance->m_controller != nullptr)
            {
                instance->m_controller->processMouseMovement(xPos, yPos);
            }
        }
    }

    void CameraSystem::mouseButtonCallback(GLFWwindow* window,
                                           const int button,
                                           const int action,
                                           const int mods)
    {
        if (s_instances.contains(window))
        {
            if (const auto instance{ s_instances.at(window) }; instance->m_controller)
            {
                instance->m_controller->processMouseButton(button, action, mods);
            }
        }
    }

    void CameraSystem::scrollCallback(GLFWwindow* window, const double xOffset, const double yOffset)
    {
        if (s_instances.contains(window))
        {
            if (const auto instance{ s_instances.at(window) };
                instance->m_controller)
            {
                instance->m_controller->processMouseScroll(xOffset, yOffset);
            }
        }
    }

    std::unordered_map<GLFWwindow*, CameraSystem*> CameraSystem::s_instances{};
} // lgl
