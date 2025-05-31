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
#include "app/PerspectiveCamera.h"
#include "app/ShaderProgram.h"
#include "app/TimeManager.h"

constexpr auto DEFAULT_WINDOW_WIDTH{ 800 };
constexpr auto DEFAULT_WINDOW_HEIGHT{ 600 };

struct DirectionalLight
{
    glm::vec3 direction;

    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
};

struct PointLight
{
    glm::vec3 position;

    glm::vec3 color;
    float ambient;
    float diffuse;
    float specular;

    float constant;
    float linear;
    float quadratic;
};

struct SpotLight
{
    glm::vec3 position;
    glm::vec3 direction;
    float cutoff;
    float outerCutoff;

    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

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

DirectionalLight directionalLight{
    .direction = { -0.2f, -1.0f, -0.3f },
    .ambient = { 0.3f, 0.24f, 0.14f },
    .diffuse = { 0.7f, 0.42f, 0.26f },
    .specular = { 0.5f, 0.5f, 0.5f },
};

PointLight pointLights[]{
    {
        .position = { 0.7f, 0.2f, 2.0f },
        .color = { 1.0f, 0.6f, 0.0f },
        .ambient = 0.1f,
        .diffuse = 1.0f,
        .specular = 1.0f,
        .constant = 1.0f,
        .linear = 0.09f,
        .quadratic = 0.032f
    },
    {
        .position = { 2.3f, -3.3f, -4.0f },
        .color = { 1.0f, 0.0f, 0.0f },
        .ambient = 0.1f,
        .diffuse = 1.0f,
        .specular = 1.0f,
        .constant = 1.0f,
        .linear = 0.09f,
        .quadratic = 0.032f
    },
    {
        .position = { -4.0f, 2.0f, -12.0f },
        .color = { 1.0f, 1.0, 0.0 },
        .ambient = 0.1f,
        .diffuse = 1.0f,
        .specular = 1.0f,
        .constant = 1.0f,
        .linear = 0.09f,
        .quadratic = 0.032f
    },
    {
        .position = { 0.0f, 0.0f, -3.0f },
        .color = { 0.2f, 0.2f, 1.0f },
        .ambient = 0.1f,
        .diffuse = 1.0f,
        .specular = 1.0f,
        .constant = 1.0f,
        .linear = 0.09f,
        .quadratic = 0.032f
    },
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

    const auto diffuseTextureImage{
        lgl::loadImageAsTexture(
            "resources/textures/container2.png"
        )
       .flipVertically()
    };
    const auto specularTextureImage{
        lgl::loadImageAsTexture(
            "resources/textures/container2_specular.png"
        )
       .flipVertically()
    };

    const auto lightingShaderProgram{
        lgl::ShaderProgram::load("shaders/illuminated_object.vert", "shaders/illuminated_object.frag")
    };
    const auto lightSourceShaderProgram{
        lgl::ShaderProgram::load("shaders/light_source.vert", "shaders/light_source.frag")
    };

    GLuint lightingVertexArrayObject{};
    glGenVertexArrays(1, &lightingVertexArrayObject);

    GLuint vertexBufferObject{};
    glGenBuffers(1, &vertexBufferObject);

    glBindVertexArray(lightingVertexArrayObject);

    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0,
                          3,
                          GL_FLOAT,
                          GL_FALSE,
                          8 * sizeof(std::ranges::range_value_t<decltype(vertices)>),
                          nullptr);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1,
                          3,
                          GL_FLOAT,
                          GL_FALSE,
                          8 * sizeof(std::ranges::range_value_t<decltype(vertices)>),
                          reinterpret_cast<void*>(3 * sizeof(std::ranges::range_value_t<decltype(vertices)>)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2,
                          2,
                          GL_FLOAT,
                          GL_FALSE,
                          8 * sizeof(std::ranges::range_value_t<decltype(vertices)>),
                          reinterpret_cast<void*>(6 * sizeof(std::ranges::range_value_t<decltype(vertices)>)));
    glEnableVertexAttribArray(2);

    GLuint diffuseMap{};
    glGenTextures(1, &diffuseMap);
    glBindTexture(GL_TEXTURE_2D, diffuseMap);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGBA,
        static_cast<GLsizei>(diffuseTextureImage.width),
        static_cast<GLsizei>(diffuseTextureImage.height),
        0,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        diffuseTextureImage.span().data()
    );
    glGenerateMipmap(GL_TEXTURE_2D);

    GLuint specularMap{};
    glGenTextures(1, &specularMap);
    glBindTexture(GL_TEXTURE_2D, specularMap);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGBA,
        static_cast<GLsizei>(specularTextureImage.width),
        static_cast<GLsizei>(specularTextureImage.height),
        0,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        specularTextureImage.span().data()
    );
    glGenerateMipmap(GL_TEXTURE_2D);

    GLuint lightSourceVertexArrayObject{};
    glGenVertexArrays(1, &lightSourceVertexArrayObject);

    glBindVertexArray(lightSourceVertexArrayObject);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);

    glVertexAttribPointer(0,
                          3,
                          GL_FLOAT,
                          GL_FALSE,
                          8 * sizeof(std::ranges::range_value_t<decltype(vertices)>),
                          nullptr);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0); // Optional

    glBindVertexArray(0); // Optional. DO NOT unbind EBO above this line or VAO will remember "NO EBO".

    lightingShaderProgram.use();
    lightingShaderProgram.setUniform("material.diffuse", 0);
    lightingShaderProgram.setUniform("material.specular", 1);

    while (!glfwWindowShouldClose(window))
    {
        timeManager.update();

        cameraSystem.update(timeManager.getDeltaTime());

        const SpotLight spotlight{
            .position = camera->getPosition(),
            .direction = camera->getForwardVector(),
            .cutoff = glm::cos(glm::radians(12.5f)),
            .outerCutoff = glm::cos(glm::radians(13.0f)),
            .ambient = { 0.0f, 0.0f, 0.0f },
            .diffuse = { 0.8f, 0.8f, 0.0f },
            .specular = { 0.8f, 0.8f, 0.0f },
            .constant = 1.0f,
            .linear = 0.09f,
            .quadratic = 0.032f
        };

        processInput(window);

        glClearColor(0.75f, 0.52f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        lightingShaderProgram.use();
        lightingShaderProgram.setUniform("material.shininess", 64.0f);
        lightingShaderProgram.setUniform("directionalLight.direction", directionalLight.direction);
        lightingShaderProgram.setUniform("directionalLight.ambient", directionalLight.ambient);
        lightingShaderProgram.setUniform("directionalLight.diffuse", directionalLight.diffuse);
        lightingShaderProgram.setUniform("directionalLight.specular", directionalLight.specular);
        for (auto&& [index, props] : std::views::enumerate(pointLights))
        {
            lightingShaderProgram.setUniform(std::format("pointLights[{}].position", index), props.position);
            lightingShaderProgram.setUniform(std::format("pointLights[{}].ambient", index),
                                             props.ambient * props.color);
            lightingShaderProgram.setUniform(std::format("pointLights[{}].diffuse", index),
                                             props.diffuse * props.color);
            lightingShaderProgram.setUniform(std::format("pointLights[{}].specular", index),
                                             props.specular * props.color);
            lightingShaderProgram.setUniform(std::format("pointLights[{}].constant", index), props.constant);
            lightingShaderProgram.setUniform(std::format("pointLights[{}].linear", index), props.linear);
            lightingShaderProgram.setUniform(std::format("pointLights[{}].quadratic", index), props.quadratic);
        }
        lightingShaderProgram.setUniform("spotLight.position", spotlight.position);
        lightingShaderProgram.setUniform("spotLight.direction", spotlight.direction);
        lightingShaderProgram.setUniform("spotLight.cutoff", spotlight.cutoff);
        lightingShaderProgram.setUniform("spotLight.outerCutoff", spotlight.outerCutoff);
        lightingShaderProgram.setUniform("spotLight.ambient", spotlight.ambient);
        lightingShaderProgram.setUniform("spotLight.diffuse", spotlight.diffuse);
        lightingShaderProgram.setUniform("spotLight.specular", spotlight.specular);
        lightingShaderProgram.setUniform("spotLight.constant", spotlight.constant);
        lightingShaderProgram.setUniform("spotLight.linear", spotlight.linear);
        lightingShaderProgram.setUniform("spotLight.quadratic", spotlight.quadratic);
        lightingShaderProgram.setUniform("viewPos", camera->getPosition());

        const auto viewMatrix{ camera->getViewMatrix() };
        const auto projectionMatrix{ camera->getProjectionMatrix() };

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffuseMap);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, specularMap);

        glBindVertexArray(lightingVertexArrayObject);

        for (auto&& [index, position] : std::views::enumerate(cubePositions))
        {
            const auto illuminatedObjectModelMatrix{
                glm::rotate(
                    glm::translate(glm::mat4{ 1.0f }, position),
                    glm::radians(20.0f * static_cast<float>(index)),
                    glm::vec3{ 1.0f, 0.3f, 0.5f }
                )
            };
            const auto normalMatrix{ glm::transpose(glm::inverse(glm::mat3{ illuminatedObjectModelMatrix })) };
            lightingShaderProgram.setUniform("normalMatrix", normalMatrix);
            lightingShaderProgram.setUniform(
                "model",
                illuminatedObjectModelMatrix
            );
            lightingShaderProgram.setUniform(
                "view",
                viewMatrix
            );
            lightingShaderProgram.setUniform(
                "projection",
                projectionMatrix
            );

            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        lightSourceShaderProgram.use();
        glBindVertexArray(lightSourceVertexArrayObject);
        for (auto&& light : pointLights)
        {
            lightSourceShaderProgram.setUniform("lightColor", light.color);
            const auto lightSourceModelMatrix{
                glm::scale(
                    glm::translate(
                        glm::mat4{ 1.0f },
                        light.position
                    ),
                    glm::vec3{ 0.2 }
                )
            };
            lightSourceShaderProgram.setUniform(
                "model",
                lightSourceModelMatrix
            );
            lightSourceShaderProgram.setUniform("view", viewMatrix);
            lightSourceShaderProgram.setUniform("projection", projectionMatrix);

            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        // Note: double buffer is used by default for modern OpenGL
        glfwSwapBuffers(window); // Swap back buffer to front as front buffer
        glfwPollEvents(); // Processes the event queue and invoke appropriate callbacks
    }

    glDeleteVertexArrays(1, &lightingVertexArrayObject);
    glDeleteBuffers(1, &lightSourceVertexArrayObject);
    glDeleteBuffers(1, &vertexBufferObject);

    glfwTerminate();

    return 0;
}
