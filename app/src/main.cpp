#include <cmath>
#include <filesystem>
#include <fstream>
#include <print>
#include <ranges>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/ext/matrix_transform.hpp>
#include "app/CameraSystem.h"
#include "app/FirstPersonController.h"
#include "app/Image.h"
#include "app/Model.h"
#include "app/PerspectiveCamera.h"
#include "app/ShaderProgram.h"
#include "app/TimeManager.h"

constexpr auto DEFAULT_WINDOW_WIDTH{ 800 };
constexpr auto DEFAULT_WINDOW_HEIGHT{ 600 };

float vertices[]{
    // positions         // normals        / texture coords
    -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,
    0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f,
    0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,
    0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,
    -0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,

    -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
    0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
    0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
    0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
    -0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
    -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,

    -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
    -0.5f, 0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
    -0.5f, -0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
    -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,

    0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
    0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
    0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
    0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
    0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
    0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,

    -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,
    0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f,
    0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
    0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
    -0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,

    -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
    0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
    0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
    0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
    -0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
    -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f
};

glm::vec3 cubePositions[]{
    { 0.0f, 0.0f, 0.0f },
    { 2.0f, 5.0f, -15.0f },
    { -1.5f, -2.2f, -2.5f },
    { -3.8f, -2.0f, -12.3f },
    { 2.4f, -0.4f, -3.5f },
    { -1.7f, 3.0f, -7.5f },
    { 1.3f, -2.0f, -2.5f },
    { 1.5f, 2.0f, -2.5f },
    { 1.5f, 0.2f, -1.5f },
    { -1.3f, 1.0f, -1.5f }
};

glm::vec3 pointLightPositions[]{
    { 0.7f, 0.2f, 2.0f },
    { 2.3f, -3.3f, -4.0f },
    { -4.0f, 2.0f, -12.0f },
    { 0.0f, 0.0f, -3.0f }
};

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
}

int main(const int argc, char* argv[])
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    auto* window{
        glfwCreateWindow(
            DEFAULT_WINDOW_WIDTH,
            DEFAULT_WINDOW_HEIGHT,
            "LearnOpenGL",
            nullptr,
            nullptr
        )
    };
    if (window == nullptr)
    {
        std::println(stderr, "Failed to create GLFW window");
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
    {
        std::println(stderr, "Failed to initialize GLAD");
        return -1;
    }

    glEnable(GL_DEPTH_TEST);

    lgl::TimeManager timeManager{};
    lgl::InputManager inputManager{ window };
    lgl::WindowManager windowManager{ window };
    lgl::CameraSystem cameraSystem{ inputManager, windowManager };

    const auto camera{ cameraSystem.getCamera<lgl::PerspectiveCamera>() };
    if (camera == nullptr)
    {
        std::println(stderr, "Failed to create camera");
        return -1;
    }

    const auto controller{ cameraSystem.getController<lgl::FirstPersonController>() };
    if (controller == nullptr)
    {
        std::println(stderr, "Failed to create controller");
        return -1;
    }

    camera->setPosition({ 0.0f, 0.0f, 3.0f });
    camera->setFov(45.0f);
    camera->setAspectRatio(static_cast<float>(DEFAULT_WINDOW_WIDTH) / static_cast<float>(DEFAULT_WINDOW_HEIGHT));

    controller->setMoveSpeed(5.0f);
    controller->setMouseSensitivity(0.1f);

    const auto backpackShaderProgram{
        lgl::ShaderProgram::load("shaders/backpack.vert", "shaders/backpack.frag")
    };

    const auto lightSourceShaderProgram{
        lgl::ShaderProgram::load("shaders/light_source.vert", "shaders/light_source.frag")
    };

    const auto backpackModel{
        lgl::Model::load("resources/models/backpack/backpack.obj")
    };

    GLuint lightSourceVertexArrayObject{};
    glGenVertexArrays(1, &lightSourceVertexArrayObject);

    GLuint vertexBufferObject{};
    glGenBuffers(1, &vertexBufferObject);

    glBindVertexArray(lightSourceVertexArrayObject);

    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0,
                          3,
                          GL_FLOAT,
                          GL_FALSE,
                          8 * sizeof(std::ranges::range_value_t<decltype(vertices)>),
                          nullptr);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0); // Optional

    glBindVertexArray(0); // Optional. DO NOT unbind EBO above this line or VAO will remember "NO EBO".

    while (!glfwWindowShouldClose(window))
    {
        timeManager.update();

        cameraSystem.update(timeManager.getDeltaTime());

        processInput(window);

        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        const auto viewMatrix{ camera->getViewMatrix() };
        const auto projectionMatrix{ camera->getProjectionMatrix() };

        backpackShaderProgram.use();
//        backpackShaderProgram.setUniform("directionalLight.direction", -0.2f, -1.0f, -0.3f);
//        backpackShaderProgram.setUniform("directionalLight.ambient", glm::vec3{ 0.05f });
//        backpackShaderProgram.setUniform("directionalLight.diffuse", glm::vec3{ 0.4f });
//        backpackShaderProgram.setUniform("directionalLight.specular", glm::vec3{ 0.5 });
//        for (auto&& [index, position] : std::views::enumerate(pointLightPositions))
//        {
//            backpackShaderProgram.setUniform(std::format("pointLights[{}].position", index), position);
//            backpackShaderProgram.setUniform(std::format("pointLights[{}].ambient", index), glm::vec3{ 0.05 });
//            backpackShaderProgram.setUniform(std::format("pointLights[{}].diffuse", index), glm::vec3{ 0.8f });
//            backpackShaderProgram.setUniform(std::format("pointLights[{}].specular", index), glm::vec3{ 1.0f });
//            backpackShaderProgram.setUniform(std::format("pointLights[{}].constant", index), 1.0f);
//            backpackShaderProgram.setUniform(std::format("pointLights[{}].linear", index), 0.09f);
//            backpackShaderProgram.setUniform(std::format("pointLights[{}].quadratic", index), 0.032f);
//        }
//        backpackShaderProgram.setUniform("spotLight.position", camera->getPosition());
//        backpackShaderProgram.setUniform("spotLight.direction", camera->getForwardVector());
//        backpackShaderProgram.setUniform("spotLight.cutoff", glm::cos(glm::radians(12.5f)));
//        backpackShaderProgram.setUniform("spotLight.outerCutoff", glm::cos(glm::radians(15.0f)));
//        backpackShaderProgram.setUniform("spotLight.ambient", glm::vec3{ 0.0f });
//        backpackShaderProgram.setUniform("spotLight.diffuse", glm::vec3{ 1.0f });
//        backpackShaderProgram.setUniform("spotLight.specular", glm::vec3{ 1.0f });
//        backpackShaderProgram.setUniform("spotLight.constant", 1.0f);
//        backpackShaderProgram.setUniform("spotLight.linear", 0.09f);
//        backpackShaderProgram.setUniform("spotLight.quadratic", 0.032f);
        backpackShaderProgram.setUniform("viewPos", camera->getPosition());

        constexpr glm::mat3 model{ 1.0f };
        backpackShaderProgram.setUniform("model", model);
        // Set view and projection matrices
        backpackShaderProgram.setUniform("view", viewMatrix);
        backpackShaderProgram.setUniform("projection", projectionMatrix);
        // Calculate and set the normal matrix
        glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(model)));
        backpackShaderProgram.setUniform("normalMatrix", normalMatrix);
        // Set material shininess
        backpackShaderProgram.setUniform("material.shininess", 64.0f);

        backpackModel.draw(backpackShaderProgram);

//        lightSourceShaderProgram.use();
//        lightSourceShaderProgram.setUniform("lightColor", glm::vec3{ 1.0 });
//        glBindVertexArray(lightSourceVertexArrayObject);
//        for (auto&& position : pointLightPositions)
//        {
//            const auto lightSourceModelMatrix{
//                glm::scale(
//                    glm::translate(
//                        glm::mat4{ 1.0f },
//                        position
//                    ),
//                    glm::vec3{ 0.2 }
//                )
//            };
//            lightSourceShaderProgram.setUniform(
//                "model",
//                lightSourceModelMatrix
//            );
//            lightSourceShaderProgram.setUniform("view", viewMatrix);
//            lightSourceShaderProgram.setUniform("projection", projectionMatrix);
//
//            glDrawArrays(GL_TRIANGLES, 0, 36);
//        }

        // Note: double buffer is used by default for modern OpenGL
        glfwSwapBuffers(window); // Swap back buffer to front as front buffer
        glfwPollEvents(); // Processes the event queue and invoke appropriate callbacks
    }

    glDeleteBuffers(1, &lightSourceVertexArrayObject);
    glDeleteBuffers(1, &vertexBufferObject);

    glfwTerminate();

    return 0;
}
