//
// Created by user on 5/28/25.
//

#ifndef LEARNOPENGL_APP_CAMERASYSTEM_H
#define LEARNOPENGL_APP_CAMERASYSTEM_H

#include <memory>
#include "app/Camera.h"
#include "app/CameraController.h"
#include "app/InputManager.h"
#include "app/WindowManager.h"

namespace lgl
{
    class CameraSystem
    {
    public:
        CameraSystem(InputManager& inputManager, WindowManager& windowManager);

        std::shared_ptr<Camera> getCamera();
        std::shared_ptr<CameraController> getController();

        template<typename T>
        std::shared_ptr<T> getCamera();

        template<typename T>
        std::shared_ptr<T> getController();

        void setCamera(const std::shared_ptr<Camera>& camera);
        void setController(const std::shared_ptr<CameraController>& controller);

        void update(float deltaTime) const;

    private:
        void setupCallbacks() const;

        std::shared_ptr<Camera> m_camera;
        std::shared_ptr<CameraController> m_controller;
        InputManager& m_inputManager;
        WindowManager& m_windowManager;
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
} // namespace lgl

#endif //LEARNOPENGL_APP_CAMERASYSTEM_H
