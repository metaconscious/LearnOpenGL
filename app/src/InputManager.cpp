//
// Created by user on 5/28/25.
//

#include "app/InputManager.h"

namespace lgl
{
    std::unordered_map<GLFWwindow*, InputManager*> InputManager::s_instances{};

    InputManager::InputManager(GLFWwindow* window)
        : m_window{ window }
    {
        s_instances.insert_or_assign(window, this);

        // Set up GLFW callbacks
        glfwSetKeyCallback(window, keyCallback);
        glfwSetCursorPosCallback(window, mouseCallback);
        glfwSetMouseButtonCallback(window, mouseButtonCallback);
        glfwSetScrollCallback(window, scrollCallback);

        // Enable raw mouse motion if supported
        if (glfwRawMouseMotionSupported())
        {
            glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
        }
    }

    InputManager::~InputManager()
    {
        s_instances.erase(m_window);
    }

    void InputManager::registerKeyCallback(std::function<void(int, int, int, int)> callback)
    {
        m_keyCallbacks.emplace_back(callback);
    }

    void InputManager::registerMouseMoveCallback(std::function<void(double, double)> callback)
    {
        m_mouseMoveCallbacks.emplace_back(callback);
    }

    void InputManager::registerMouseButtonCallback(std::function<void(int, int, int)> callback)
    {
        m_mouseButtonCallbacks.emplace_back(callback);
    }

    void InputManager::registerScrollCallback(std::function<void(double, double)> callback)
    {
        m_scrollCallbacks.emplace_back(callback);
    }

    void InputManager::setCursorMode(const int mode) const
    {
        glfwSetInputMode(m_window, GLFW_CURSOR, mode);
    }

    int InputManager::getCursorMode() const
    {
        return glfwGetInputMode(m_window, GLFW_CURSOR);
    }

    void InputManager::keyCallback(GLFWwindow* window,
                                   const int key,
                                   const int scancode,
                                   const int action,
                                   const int mods)
    {
        if (s_instances.contains(window))
        {
            const auto instance{ s_instances.at(window) };
            for (const auto& callbacks{ instance->m_keyCallbacks };
                 const auto& callback : callbacks)
            {
                std::invoke(callback, key, scancode, action, mods);
            }
        }
    }

    void InputManager::mouseCallback(GLFWwindow* window, double xPos, double yPos)
    {
        if (s_instances.contains(window))
        {
            const auto instance{ s_instances.at(window) };
            for (const auto& callbacks{ instance->m_mouseMoveCallbacks };
                 const auto& callback : callbacks)
            {
                std::invoke(callback, xPos, yPos);
            }
        }
    }

    void InputManager::mouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
    {
        if (s_instances.contains(window))
        {
            const auto instance{ s_instances.at(window) };
            for (const auto& callbacks{ instance->m_mouseButtonCallbacks };
                 const auto& callback : callbacks)
            {
                std::invoke(callback, button, action, mods);
            }
        }
    }

    void InputManager::scrollCallback(GLFWwindow* window, double xOffset, double yOffset)
    {
        if (s_instances.contains(window))
        {
            const auto instance{ s_instances.at(window) };
            for (const auto& callbacks{ instance->m_scrollCallbacks };
                 const auto& callback : callbacks)
            {
                std::invoke(callback, xOffset, yOffset);
            }
        }
    }
} // namespace lgl
