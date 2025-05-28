//
// Created by user on 5/28/25.
//

#ifndef LEARNOPENGL_APP_WINDOWMANAGER_H
#define LEARNOPENGL_APP_WINDOWMANAGER_H

#include <functional>
#include <unordered_map>
#include <vector>
#include <GLFW/glfw3.h>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

namespace lgl
{
    class WindowManager
    {
    public:
        explicit WindowManager(GLFWwindow* window);
        ~WindowManager();

        void updateViewport(int width, int height);
        void registerResizeCallback(std::function<void(int, int)> callback);

        // Window properties
        glm::vec2 getWindowSize() const;
        float getAspectRatio() const;

    private:
        static void framebufferSizeCallback(GLFWwindow* window, int width, int height);

        GLFWwindow* m_window;
        glm::vec2 m_windowSize;
        static std::unordered_map<GLFWwindow*, WindowManager*> s_instances;
        std::vector<std::function<void(int, int)>> m_resizeCallbacks;
    };
} // namespace lgl

#endif //LEARNOPENGL_APP_WINDOWMANAGER_H
