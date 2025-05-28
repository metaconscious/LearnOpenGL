//
// Created by user on 5/28/25.
//

#ifndef LEARNOPENGL_APP_FIRSTPERSONCONTROLLER_H
#define LEARNOPENGL_APP_FIRSTPERSONCONTROLLER_H

#include <memory>
#include <unordered_map>
#include <GLFW/glfw3.h>
#include "app/PerspectiveCamera.h"
#include "app/CameraController.h"

namespace lgl
{
    class FirstPersonController final : public CameraController
    {
    public:
        FirstPersonController() = default;

        void setCamera(std::shared_ptr<Camera> camera) override;

        void update(float deltaTime) override;

        void processKeyInput(int key, int scancode, int action, int mods) override;

        void processMouseMovement(double xPos, double yPos) override;

        void processMouseButton(int button, int action, int mods) override;

        void processMouseScroll(double xOffset, double yOffset) override;

        bool handleCursorToggle(int key, int action, int mods) override;

        void bindKey(int key, KeyActionCallback action) override;

        // Getters and setters for controller properties
        void setMoveSpeed(float speed);

        float getMoveSpeed() const;

        void setMouseSensitivity(float sensitivity);

        float getMouseSensitivity() const;

        void setKeyMapping(int forwardKey,
                           int backwardKey,
                           int leftKey,
                           int rightKey,
                           int upKey,
                           int downKey,
                           int cursorToggleKey);

        bool isCursorEnabled() const;

        void enableCursor(bool enable);

    private:
        void updateCameraRotation() const;

        void setupDefaultBindings();

    private:
        std::shared_ptr<Camera> m_camera;

        // Movement settings
        float m_moveSpeed{ 5.0f };
        float m_mouseSensitivity{ 0.1f };

        // Input state
        std::unordered_map<int, bool> m_keyStates;
        glm::dvec2 m_lastMousePos{ 0.0, 0.0 };
        bool m_firstMouse{ true };
        bool m_cursorEnabled{ false };

        // Camera orientation
        float m_pitch{ 0.0f };
        float m_yaw{ -90.0f }; // -90 to face along negative Z axis initially

        // Key bindings
        std::unordered_map<int, KeyActionCallback> m_keyBindings;

        // Default movement keys
        int m_forwardKey{ GLFW_KEY_W };
        int m_backwardKey{ GLFW_KEY_S };
        int m_leftKey{ GLFW_KEY_A };
        int m_rightKey{ GLFW_KEY_D };
        int m_upKey{ GLFW_KEY_SPACE };
        int m_downKey{ GLFW_KEY_LEFT_SHIFT };
        int m_cursorToggleKey{ GLFW_KEY_LEFT_ALT };
    };
} // lgl

#endif //LEARNOPENGL_APP_FIRSTPERSONCONTROLLER_H
