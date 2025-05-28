//
// Created by user on 5/28/25.
//

#include "app/WindowManager.h"

namespace lgl
{
    std::unordered_map<GLFWwindow*, WindowManager*> WindowManager::s_instances{};

    WindowManager::WindowManager(GLFWwindow* window)
        : m_window{ window }
    {
        s_instances.insert_or_assign(window, this);

        // Get initial window size
        int width{};
        int height{};
        glfwGetFramebufferSize(window, &width, &height);
        m_windowSize = { width, height };

        // Set up resize callback
        glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    }

    WindowManager::~WindowManager()
    {
        s_instances.erase(m_window);
    }

    void WindowManager::updateViewport(const int width, const int height)
    {
        m_windowSize = { width, height };
        glViewport(0, 0, width, height);
    }

    void WindowManager::registerResizeCallback(std::function<void(int, int)> callback)
    {
        m_resizeCallbacks.emplace_back(callback);
    }

    glm::vec2 WindowManager::getWindowSize() const
    {
        return m_windowSize;
    }

    float WindowManager::getAspectRatio() const
    {
        return m_windowSize.x > 0 && m_windowSize.y > 0
               ? m_windowSize.x / m_windowSize.y
               : 1.0f;
    }

    void WindowManager::framebufferSizeCallback(GLFWwindow* window, const int width, const int height)
    {
        if (s_instances.contains(window))
        {
            const auto instance{ s_instances.at(window) };

            // Update viewport
            instance->updateViewport(width, height);

            // Notify all registered callbacks
            for (const auto& callback : instance->m_resizeCallbacks)
            {
                callback(width, height);
            }
        }
    }
} // namespace lgl
