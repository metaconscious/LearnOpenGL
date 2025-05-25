//
// Created by user on 5/23/25.
//

#ifndef LEARNOPENGL_APP_CAMERA_H
#define LEARNOPENGL_APP_CAMERA_H

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
    // Constants to replace magic literals
    namespace constants
    {
        // Default camera values
        constexpr auto DEFAULT_YAW{ -90.0f };
        constexpr auto DEFAULT_PITCH{ 0.0f };
        constexpr auto DEFAULT_ROLL{ 0.0f };
        constexpr auto DEFAULT_MOVEMENT_SPEED{ 5.0f };
        constexpr auto DEFAULT_MOUSE_SENSITIVITY{ 0.1f };
        constexpr auto DEFAULT_FOV{ 45.0f };
        constexpr auto DEFAULT_ASPECT_RATIO{ 16.0f / 9.0f };
        constexpr auto DEFAULT_NEAR_PLANE{ 0.1f };
        constexpr auto DEFAULT_FAR_PLANE{ 1000.0f };
        constexpr auto DEFAULT_ORBIT_DISTANCE{ 10.0f };

        // Constraints
        constexpr auto MIN_PITCH{ -89.0f };
        constexpr auto MAX_PITCH{ 89.0f };
        constexpr auto MIN_FOV{ 1.0f };
        constexpr auto MAX_FOV{ 170.0f };
        constexpr auto MIN_ORBIT_DISTANCE{ 0.1f };

        // Vectors
        constexpr glm::vec3 WORLD_UP{ 0.0f, 1.0f, 0.0f };
        constexpr glm::vec3 WORLD_FORWARD{ 0.0f, 0.0f, -1.0f };
        constexpr glm::vec3 WORLD_RIGHT{ 1.0f, 0.0f, 0.0f };
        constexpr glm::vec3 DEFAULT_POSITION{ 0.0f, 0.0f, 3.0f };
    }

    // Forward declarations
    class Spatial;

    // Transform component for hierarchical transformations
    class Transform
    {
    public:
        Transform();
        explicit Transform(const glm::vec3& position,
                           const glm::quat& rotation = { 1.0f, 0.0f, 0.0f, 0.0f },
                           const glm::vec3& scale = glm::vec3{ 1.0f });

        // Local transformations
        void setLocalPosition(const glm::vec3& position);
        void setLocalRotation(const glm::quat& rotation);
        void setLocalScale(const glm::vec3& scale);

        [[nodiscard]] const glm::vec3& getLocalPosition() const;
        [[nodiscard]] const glm::quat& getLocalRotation() const;
        [[nodiscard]] const glm::vec3& getLocalScale() const;

        // World transformations
        [[nodiscard]] glm::vec3 getWorldPosition() const;
        [[nodiscard]] glm::quat getWorldRotation() const;
        [[nodiscard]] glm::vec3 getWorldScale() const;

        void setWorldPosition(const glm::vec3& position);
        void setWorldRotation(const glm::quat& rotation);

        // Hierarchy
        void setParent(Transform* parent);
        [[nodiscard]] Transform* getParent() const;
        void addChild(Transform* child);
        void removeChild(const Transform* child);
        [[nodiscard]] const std::vector<Transform*>& getChildren() const;

        // Matrices
        [[nodiscard]] glm::mat4 getLocalMatrix() const;
        [[nodiscard]] glm::mat4 getWorldMatrix() const;

        // Direction vectors in world space
        [[nodiscard]] glm::vec3 forward() const;
        [[nodiscard]] glm::vec3 up() const;
        [[nodiscard]] glm::vec3 right() const;

        // Utility methods
        void lookAt(const glm::vec3& target, const glm::vec3& up = constants::WORLD_UP);

    private:
        glm::vec3 m_localPosition;
        glm::quat m_localRotation;
        glm::vec3 m_localScale;

        Transform* m_parent;
        std::vector<Transform*> m_children;

        mutable bool m_worldMatrixDirty;
        mutable glm::mat4 m_worldMatrix{};

        void markDirty() const;
        void updateWorldMatrix() const;
    };

    // Base class for objects with position and orientation in 3D space
    class Spatial
    {
    public:
        // Constructor with default position and orientation
        explicit Spatial(const glm::vec3& position = constants::DEFAULT_POSITION,
                         float yaw = constants::DEFAULT_YAW,
                         float pitch = constants::DEFAULT_PITCH,
                         float roll = constants::DEFAULT_ROLL,
                         const glm::vec3& worldUp = constants::WORLD_UP);

        // Copy/move constructors and assignment operators
        Spatial(const Spatial&) = default;
        Spatial(Spatial&&) noexcept = default;
        Spatial& operator=(const Spatial&) = default;
        Spatial& operator=(Spatial&&) noexcept = default;

        virtual ~Spatial() = default;

        // Transform methods
        [[nodiscard]] Transform& getTransform();
        [[nodiscard]] const Transform& getTransform() const;

        // Legacy position methods (delegates to transform)
        void setPosition(const glm::vec3& position);
        [[nodiscard]] const glm::vec3& getPosition() const;

        // Legacy orientation methods (delegates to transform)
        void setOrientation(float yaw, float pitch, float roll = 0.0f);
        [[nodiscard]] float getYaw() const;
        [[nodiscard]] float getPitch() const;
        [[nodiscard]] float getRoll() const;

        // Legacy direction vectors (delegates to transform)
        [[nodiscard]] glm::vec3 getForward() const;
        [[nodiscard]] glm::vec3 getUp() const;
        [[nodiscard]] glm::vec3 getRight() const;
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
        void rotateAround(const glm::vec3& point, const glm::vec3& axis, float angle);

        // Matrix transformation
        [[nodiscard]] glm::mat4 getModelMatrix() const;

        // Path following
        void followPath(const std::vector<glm::vec3>& path, float t);

        // Interpolation
        void interpolateTo(const Spatial& target, float t);

    protected:
        // Core spatial properties
        Transform m_transform;
        float m_movementSpeed{ constants::DEFAULT_MOVEMENT_SPEED };

        // Euler angles cache (for legacy support)
        struct EulerAngles
        {
            float yaw{ constants::DEFAULT_YAW };
            float pitch{ constants::DEFAULT_PITCH };
            float roll{ constants::DEFAULT_ROLL };
        };

        EulerAngles m_eulerAngles;

        // Update Euler angles from quaternion
        void updateEulerAngles();

        // Update quaternion from Euler angles
        void updateQuaternion();

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
        bool m_actionStates[static_cast<size_t>(CameraAction::ZoomOut) + 1] = { false };
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
