//
// Created by user on 5/28/25.
//

#ifndef LEARNOPENGL_APP_INPUTMANAGER_H
#define LEARNOPENGL_APP_INPUTMANAGER_H

#include <functional>
#include <unordered_map>
#include <vector>
#include <GLFW/glfw3.h>

namespace lgl
{
    class InputManager
    {
    public:
        explicit InputManager(GLFWwindow* window);
        ~InputManager();

        // Registration for callbacks
        void registerKeyCallback(std::function<void(int, int, int, int)> callback);
        void registerMouseMoveCallback(std::function<void(double, double)> callback);
        void registerMouseButtonCallback(std::function<void(int, int, int)> callback);
        void registerScrollCallback(std::function<void(double, double)> callback);

        // Cursor state management
        void setCursorMode(int mode) const;
        int getCursorMode() const;

    private:
        // GLFW callback handling
        static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
        static void mouseCallback(GLFWwindow* window, double xPos, double yPos);
        static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
        static void scrollCallback(GLFWwindow* window, double xOffset, double yOffset);

        // Member variables
        GLFWwindow* m_window;
        static std::unordered_map<GLFWwindow*, InputManager*> s_instances;

        // Registered callbacks
        std::vector<std::function<void(int, int, int, int)>> m_keyCallbacks;
        std::vector<std::function<void(double, double)>> m_mouseMoveCallbacks;
        std::vector<std::function<void(int, int, int)>> m_mouseButtonCallbacks;
        std::vector<std::function<void(double, double)>> m_scrollCallbacks;
    };
} // namespace lgl

#endif //LEARNOPENGL_APP_INPUTMANAGER_H
