//
// Created by user on 5/28/25.
//

#ifndef LEARNOPENGL_APP_CAMERASYSTEM_H
#define LEARNOPENGL_APP_CAMERASYSTEM_H

#include <chrono>
#include <memory>
#include <unordered_map>
#include <GLFW/glfw3.h>
#include "app/FirstPersonController.h"
#include "app/PerspectiveCamera.h"

namespace lgl
{
    class CameraSystem
    {
    public:
        explicit CameraSystem(GLFWwindow* window);

        ~CameraSystem();

        std::shared_ptr<Camera> getCamera();

        template<typename T>
        std::shared_ptr<T> getCamera();

        std::shared_ptr<CameraController> getController();

        template<typename T>
        std::shared_ptr<T> getController();

        void setCamera(const std::shared_ptr<Camera>& camera);

        void setController(const std::shared_ptr<CameraController>& controller);

        void update();

    private:
        void setupCallbacks() const;

        // Static callback functions
        static void framebufferSizeCallback(GLFWwindow* window, int width, int height);

        static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

        static void mouseCallback(GLFWwindow* window, double xPos, double yPos);

        static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);

        static void scrollCallback(GLFWwindow* window, double xOffset, double yOffset);


        GLFWwindow* m_window;
        std::shared_ptr<Camera> m_camera;
        std::shared_ptr<CameraController> m_controller;

        // Timing
        std::chrono::time_point<std::chrono::high_resolution_clock> m_lastFrameTime;
        float m_deltaTime{ 0.0f };

        // Callback pointers for GLFW
        static std::unordered_map<GLFWwindow*, CameraSystem*> s_instances;
    };

    template<typename T>
    std::shared_ptr<T> CameraSystem::getCamera()
    {
        return std::dynamic_pointer_cast<T>(m_camera);
    }

    template<typename T>
    std::shared_ptr<T> CameraSystem::getController()
    {
        return std::dynamic_pointer_cast<T>(m_controller);
    }
} // lgl

#endif //LEARNOPENGL_APP_CAMERASYSTEM_H
