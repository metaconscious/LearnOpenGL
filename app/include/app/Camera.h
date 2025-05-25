//
// Created by user on 5/23/25.
//

#ifndef LEARNOPENGL_APP_CAMERA_H
#define LEARNOPENGL_APP_CAMERA_H

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <memory>
#include <optional>
#include <string>

namespace lgl
{
    enum class CameraType
    {
        Perspective,
        Orthographic
    };

    enum class CameraMode
    {
        FirstPerson,
        Orbital,
        Free
    };

    // Forward declaration
    class CameraController;

    class Camera
    {
    public:
        struct CameraSettings
        {
            float fieldOfView{ 45.0f };
            float aspectRatio{ 16.0f / 9.0f };
            float nearPlane{ 0.1f };
            float farPlane{ 1000.0f };
            float movementSpeed{ 5.0f };
            float mouseSensitivity{ 0.1f };
            CameraType type{ CameraType::Perspective };
            CameraMode mode{ CameraMode::Free };
        };

        static const CameraSettings DEFAULT_CAMERA_SETTINGS;

    private:
        // Core camera properties
        glm::vec3 m_position{ 0.0f, 0.0f, 3.0f };
        glm::vec3 m_forward{ 0.0f, 0.0f, -1.0f };
        glm::vec3 m_up{ 0.0f, 1.0f, 0.0f };
        glm::vec3 m_right{ 1.0f, 0.0f, 0.0f };
        glm::vec3 m_worldUp{ 0.0f, 1.0f, 0.0f };

        // Euler angles
        float m_yaw{ -90.0f }; // Yaw is initialized to -90 to point toward negative Z
        float m_pitch{ 0.0f };
        float m_roll{ 0.0f };

        // Camera parameters
        CameraSettings m_settings;

        // Matrices
        mutable bool m_viewDirty{ true };
        mutable bool m_projectionDirty{ true };
        mutable glm::mat4 m_viewMatrix{ 1.0f };
        mutable glm::mat4 m_projectionMatrix{ 1.0f };

        // Target (for orbital camera)
        std::optional<glm::vec3> m_target;
        float m_orbitDistance{ 10.0f };

        // Controller
        std::unique_ptr<CameraController> m_controller;

        // Update camera vectors based on Euler angles
        void updateCameraVectors();

    public:
        explicit Camera(const CameraSettings& settings = DEFAULT_CAMERA_SETTINGS);

        // Create camera with position and target
        static Camera createLookAt(const glm::vec3& position,
                                   const glm::vec3& target,
                                   const glm::vec3& up = { 0.0f, 1.0f, 0.0f });

        // Get view matrix
        [[nodiscard]] const glm::mat4& getViewMatrix() const;

        // Get projection matrix
        [[nodiscard]] const glm::mat4& getProjectionMatrix() const;

        // Movement methods
        void moveForward(float distance);

        void moveRight(float distance);

        void moveUp(float distance);

        // Rotation methods
        void rotate(float yawOffset, float pitchOffset);

        void setRoll(float roll);

        // Set target for orbital camera
        void setTarget(const glm::vec3& target);

        void clearTarget();

        void setOrbitDistance(float distance);

        // Configuration methods
        void setFieldOfView(float fov);

        void setAspectRatio(float aspectRatio);

        void setNearPlane(float nearPlane);

        void setFarPlane(float farPlane);

        void setCameraType(CameraType type);

        void setCameraMode(CameraMode mode);

        // Getters
        [[nodiscard]] const glm::vec3& getPosition() const;

        [[nodiscard]] const glm::vec3& getForward() const;

        [[nodiscard]] const glm::vec3& getUp() const;

        [[nodiscard]] const glm::vec3& getRight() const;

        [[nodiscard]] float getYaw() const;

        [[nodiscard]] float getPitch() const;

        [[nodiscard]] float getRoll() const;

        // Setters with vector
        void setPosition(const glm::vec3& position);

        void setOrientation(float yaw, float pitch);

        void setWorldUp(const glm::vec3& up);

        // Other getters
        [[nodiscard]] const CameraSettings& settings() const;

        [[nodiscard]] std::optional<glm::vec3> optionalTarget() const;

        // Frustum extraction for culling
        struct Frustum
        {
            enum Planes { Right, Left, Bottom, Top, Far, Near };

            std::array<glm::vec4, 6> planes;
        };

        Frustum extractFrustum() const;

        // Check if a point is visible
        static bool isPointVisible(const glm::vec3& point, const Frustum& frustum);

        // Check if a sphere is visible
        static bool isSphereVisible(const glm::vec3& center, float radius, const Frustum& frustum);

        // Check if an axis-aligned bounding box is visible
        static bool isAABBVisible(const glm::vec3& min, const glm::vec3& max, const Frustum& frustum);

        // Utility methods
        [[nodiscard]] glm::vec3 screenToWorld(const glm::vec2& screenPos, const glm::vec2& screenSize) const;

        // Ray casting for picking
        struct Ray
        {
            glm::vec3 origin;
            glm::vec3 direction;
        };

        [[nodiscard]] Ray createRayFromScreen(const glm::vec2& screenPos, const glm::vec2& screenSize) const;

        // Serialization support
        [[nodiscard]] std::string serialize() const;

        // Animation support
        void interpolateTo(const Camera& target, float t);
    };

    // Camera controller interface
    class CameraController
    {
    public:
        virtual ~CameraController() = default;
        virtual void update(Camera& camera, float deltaTime) = 0;
        virtual void processKeyInput(Camera& camera, int key, int action) = 0;
        virtual void processMouseMovement(Camera& camera, float xOffset, float yOffset) = 0;
        virtual void processMouseScroll(Camera& camera, float yOffset) = 0;
    };

    // First-person camera controller
    class FirstPersonController : public CameraController
    {
    private:
        bool m_keys[1024] = { false };
        bool m_firstMouse{ true };
        float m_lastX{ 0.0f };
        float m_lastY{ 0.0f };

    public:
        explicit FirstPersonController(float initialX = 0.0f, float initialY = 0.0f);

        void update(Camera& camera, float deltaTime) override;

        void processKeyInput(Camera& camera, int key, int action) override;

        void processMouseMovement(Camera& camera, float xPos, float yPos) override;

        void processMouseScroll(Camera& camera, float yOffset) override;
    };

    // Orbital camera controller
    class OrbitalController : public CameraController
    {
    private:
        bool m_rotating{ false };
        float m_lastX{ 0.0f };
        float m_lastY{ 0.0f };

    public:
        explicit OrbitalController(float initialX = 0.0f, float initialY = 0.0f);

        void update(Camera& camera, float deltaTime) override;

        void processKeyInput(Camera& camera, int key, int action) override;

        void processMouseMovement(Camera& camera, float xPos, float yPos) override;

        void processMouseScroll(Camera& camera, float yOffset) override;

        void setRotating(bool rotating);
    };

    // Example usage with GLFW window
    class CameraSystem
    {
    private:
        Camera m_camera;
        std::unique_ptr<CameraController> m_controller;
        GLFWwindow* m_window;

        static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

        static void mouseCallback(GLFWwindow* window, double xPos, double yPos);

        static void scrollCallback(GLFWwindow* window, double xOffset, double yOffset);

    public:
        explicit CameraSystem(GLFWwindow* window, const Camera::CameraSettings& settings = {});

        void update(float deltaTime);

        [[nodiscard]] Camera& getCamera();

        [[nodiscard]] const Camera& getCamera() const;
    };
} // lgl

#endif //LEARNOPENGL_APP_CAMERA_H
