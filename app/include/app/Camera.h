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
    // Base class for objects with position and orientation in 3D space
    class Spatial
    {
    public:
        // Constructor with default position and orientation
        explicit Spatial(const glm::vec3& position = { 0.0f, 0.0f, 0.0f },
                         float yaw = -90.0f,
                         float pitch = 0.0f,
                         float roll = 0.0f,
                         const glm::vec3& worldUp = { 0.0f, 1.0f, 0.0f });

        // Copy/move constructors and assignment operators
        Spatial(const Spatial&) = default;
        Spatial(Spatial&&) noexcept = default;
        Spatial& operator=(const Spatial&) = default;
        Spatial& operator=(Spatial&&) noexcept = default;

        virtual ~Spatial() = default;
        // Position methods
        void setPosition(const glm::vec3& position);
        [[nodiscard]] const glm::vec3& getPosition() const;

        // Orientation methods
        void setOrientation(float yaw, float pitch, float roll = 0.0f);
        [[nodiscard]] float getYaw() const;
        [[nodiscard]] float getPitch() const;
        [[nodiscard]] float getRoll() const;

        // Direction vectors
        [[nodiscard]] const glm::vec3& getForward() const;
        [[nodiscard]] const glm::vec3& getUp() const;
        [[nodiscard]] const glm::vec3& getRight() const;
        void setWorldUp(const glm::vec3& up);

        // Movement methods
        void moveForward(float distance);
        void moveRight(float distance);
        void moveUp(float distance);
        void moveInDirection(const glm::vec3& direction, float distance);

        [[nodiscard]] float movementSpeed() const;
        void setMovementSpeed(float movementSpeed);

        // Rotation methods
        void rotate(float yawOffset, float pitchOffset);

        // Matrix transformation
        [[nodiscard]] glm::mat4 getModelMatrix() const;

    protected:
        // Core spatial properties
        struct State
        {
            glm::vec3 position{ 0.0f, 0.0f, 0.0f };
            glm::vec3 forward{ 0.0f, 0.0f, -1.0f };
            glm::vec3 up{ 0.0f, 1.0f, 0.0f };
            glm::vec3 right{ 1.0f, 0.0f, 0.0f };
            glm::vec3 worldUp{ 0.0f, 1.0f, 0.0f };
            float yaw{ -90.0f };
            float pitch{ 0.0f };
            float roll{ 0.0f };
            float movementSpeed{ 5.0f };
        };

        State m_state;

        // Update direction vectors based on Euler angles
        void updateVectors();

        // Notify derived classes that spatial state has changed
        virtual void onSpatialChanged();
    };

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

    class Camera final : public Spatial
    {
    public:
        struct CameraSettings
        {
            float fieldOfView{ 45.0f };
            float aspectRatio{ 16.0f / 9.0f };
            float nearPlane{ 0.1f };
            float farPlane{ 1000.0f };
            float mouseSensitivity{ 0.1f };
            CameraType type{ CameraType::Perspective };
            CameraMode mode{ CameraMode::Free };
        };

        static const CameraSettings DEFAULT_CAMERA_SETTINGS;

    private:
        // Camera-specific parameters
        CameraSettings m_settings;

        // Target (for orbital camera)
        std::optional<glm::vec3> m_target;
        float m_orbitDistance{ 10.0f };

        // Controller
        std::unique_ptr<CameraController> m_controller;

        // Cache for matrices
        struct Cache
        {
            bool viewDirty{ true };
            bool projectionDirty{ true };
            glm::mat4 viewMatrix{ 1.0f };
            glm::mat4 projectionMatrix{ 1.0f };
        };

        std::unique_ptr<Cache> m_cache;

        // Override from Spatial
        void onSpatialChanged() override;

    public:
        explicit Camera(const CameraSettings& settings = DEFAULT_CAMERA_SETTINGS);

        // Copy/move constructors and assignment operators
        Camera(const Camera& other);
        Camera(Camera&& other) noexcept;
        Camera& operator=(const Camera& other);
        Camera& operator=(Camera&& other) noexcept;

        ~Camera() override = default;

        // Create camera with position and target
        static Camera createLookAt(const glm::vec3& position,
                                   const glm::vec3& target,
                                   const glm::vec3& up = { 0.0f, 1.0f, 0.0f });

        // Get view matrix
        [[nodiscard]] const glm::mat4& getViewMatrix() const;

        // Get projection matrix
        [[nodiscard]] const glm::mat4& getProjectionMatrix() const;

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

        // Other getters
        [[nodiscard]] const CameraSettings& settings() const;
        [[nodiscard]] std::optional<glm::vec3> optionalTarget() const;

        // Frustum extraction for culling
        struct Frustum
        {
            enum Planes { Right, Left, Bottom, Top, Far, Near };

            std::array<glm::vec4, 6> planes;
        };

        [[nodiscard]] Frustum extractFrustum() const;
        // Culling methods
        static bool isPointVisible(const glm::vec3& point, const Frustum& frustum);
        static bool isSphereVisible(const glm::vec3& center, float radius, const Frustum& frustum);
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
