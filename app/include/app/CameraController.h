//
// Created by user on 5/28/25.
//

#ifndef LEARNOPENGL_APP_CAMERACONTROLLER_H
#define LEARNOPENGL_APP_CAMERACONTROLLER_H

#include <functional>
#include <memory>
#include "app/Camera.h"

namespace lgl
{
    class CameraController
    {
    public:
        virtual ~CameraController() = default;
        virtual void update(float deltaTime) = 0;
        virtual void processKeyInput(int key, int scancode, int action, int mods) = 0;
        virtual void processMouseMovement(double xPos, double yPos) = 0;
        virtual void processMouseButton(int button, int action, int mods) = 0;
        virtual void processMouseScroll(double xOffset, double yOffset) = 0;
        virtual void setCamera(std::shared_ptr<Camera> camera) = 0;
        virtual bool handleCursorToggle(int key, int action, int mods) = 0;

        // Custom key binding interface
        using KeyActionCallback = std::function<void(float)>;
        virtual void bindKey(int key, KeyActionCallback action) = 0;
    };
} // lgl

#endif //LEARNOPENGL_APP_CAMERACONTROLLER_H
