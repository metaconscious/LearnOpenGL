//
// Created by user on 5/23/25.
//

#ifndef LEARNOPENGL_APP_CAMERA_H
#define LEARNOPENGL_APP_CAMERA_H

#include "app/constants.h"
#include "app/Spatial.h"

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

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

    // Input action mapping
    enum class CameraAction
    {
        MoveForward,
        MoveBackward,
        MoveLeft,
        MoveRight,
        MoveUp,
        MoveDown,
        RotateCamera,
        ZoomIn,
        ZoomOut
    };

    // Forward declaration
    class CameraController;

    class Camera final : public Spatial
    {
    public:
        struct CameraSettings
        {
            float fieldOfView{ constants::DEFAULT_FOV };
            float aspectRatio{ constants::DEFAULT_ASPECT_RATIO };
            float nearPlane{ constants::DEFAULT_NEAR_PLANE };
            float farPlane{ constants::DEFAULT_FAR_PLANE };
            float mouseSensitivity{ constants::DEFAULT_MOUSE_SENSITIVITY };
            CameraType type{ CameraType::Perspective };
            CameraMode mode{ CameraMode::Free };
        };

        static const CameraSettings DEFAULT_CAMERA_SETTINGS;

    private:
        // Camera-specific parameters
        CameraSettings m_settings;

        // Target (for orbital camera)
        std::optional<glm::vec3> m_target;
        float m_orbitDistance{ constants::DEFAULT_ORBIT_DISTANCE };

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
                                   const glm::vec3& up = constants::WORLD_UP);

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
        void setMouseSensitivity(float sensitivity);

        // Other getters
        [[nodiscard]] const CameraSettings& settings() const;
        [[nodiscard]] std::optional<glm::vec3> optionalTarget() const;
        [[nodiscard]] float getMouseSensitivity() const;

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

    // Input binding configuration
    struct InputBinding
    {
        int key{ GLFW_KEY_UNKNOWN };
        int mouseButton{ -1 };
        bool isMouseMovement{ false };
        bool isScrollWheel{ false };
        CameraAction action{ CameraAction::MoveForward };
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
        virtual void processMouseButton(Camera& camera, int button, int action) = 0;

        // Input binding configuration
        void setInputBindings(const std::vector<InputBinding>& bindings);
        [[nodiscard]] const std::vector<InputBinding>& getInputBindings() const;

    protected:
        std::vector<InputBinding> m_inputBindings;
        std::unordered_map<int, CameraAction> m_keyBindings;
        std::unordered_map<int, CameraAction> m_mouseButtonBindings;
        std::optional<CameraAction> m_mouseMovementBinding;
        std::optional<CameraAction> m_scrollWheelBinding;

        void rebuildBindingMaps();
    };

    // First-person camera controller
    class FirstPersonController final : public CameraController
    {
    public:
        explicit FirstPersonController(float initialX = 0.0f, float initialY = 0.0f);

        void update(Camera& camera, float deltaTime) override;
        void processKeyInput(Camera& camera, int key, int action) override;
        void processMouseMovement(Camera& camera, float xPos, float yPos) override;
        void processMouseScroll(Camera& camera, float yOffset) override;
        void processMouseButton(Camera& camera, int button, int action) override;

        // Set default bindings
        void setDefaultBindings();

    private:
        bool m_keys[1024] = { false };
        bool m_firstMouse{ true };
        float m_lastX{ 0.0f };
        float m_lastY{ 0.0f };
        std::array<bool, static_cast<size_t>(CameraAction::ZoomOut) + 1> m_actionStates{};
    };

    // Orbital camera controller
    class OrbitalController final : public CameraController
    {
    public:
        explicit OrbitalController(float initialX = 0.0f, float initialY = 0.0f);

        void update(Camera& camera, float deltaTime) override;
        void processKeyInput(Camera& camera, int key, int action) override;
        void processMouseMovement(Camera& camera, float xPos, float yPos) override;
        void processMouseScroll(Camera& camera, float yOffset) override;
        void processMouseButton(Camera& camera, int button, int action) override;

        void setRotating(bool rotating);

        // Set default bindings
        void setDefaultBindings();

    private:
        bool m_rotating{ false };
        float m_lastX{ 0.0f };
        float m_lastY{ 0.0f };
        std::array<bool, static_cast<size_t>(CameraAction::ZoomOut) + 1> m_actionStates{};
    };

    // Example usage with GLFW window
    class CameraSystem
    {
    public:
        explicit CameraSystem(GLFWwindow* window, const Camera::CameraSettings& settings = {});

        void update();
        [[nodiscard]] Camera& getCamera();
        [[nodiscard]] const Camera& getCamera() const;

        // Configure input bindings
        void setInputBindings(const std::vector<InputBinding>& bindings) const;

    private:
        Camera m_camera;
        std::unique_ptr<CameraController> m_controller;
        GLFWwindow* m_window;
        float m_lastFrameTime{ 0.0f };

        static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
        static void mouseCallback(GLFWwindow* window, double xPos, double yPos);
        static void scrollCallback(GLFWwindow* window, double xOffset, double yOffset);
        static void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
    };
} // lgl

#endif //LEARNOPENGL_APP_CAMERA_H
