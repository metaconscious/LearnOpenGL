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
    const auto emissiveTextureImage{
        lgl::loadImageAsTexture(
            "resources/textures/matrix.jpg"
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

    GLuint emissiveTexture{};
    glGenTextures(1, &emissiveTexture);
    glBindTexture(GL_TEXTURE_2D, emissiveTexture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGB,
        static_cast<GLsizei>(emissiveTextureImage.width),
        static_cast<GLsizei>(emissiveTextureImage.height),
        0,
        GL_RGB,
        GL_UNSIGNED_BYTE,
        emissiveTextureImage.span().data()
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

    constexpr glm::vec3 lightPos{ 1.2f, 1.0f, 2.0f };

    lightingShaderProgram.use();
    lightingShaderProgram.setUniform("material.diffuse", 0);
    lightingShaderProgram.setUniform("material.specular", 1);
    lightingShaderProgram.setUniform("material.emissive", 2);

    while (!glfwWindowShouldClose(window))
    {
        glm::vec3 lightColor{ 1.0f, 1.0f, 1.0f };

        glm::vec3 ambientColor{ lightColor * glm::vec3{ 0.2f } };
        glm::vec3 diffuseColor{ lightColor * glm::vec3{ 0.5f } };

        timeManager.update();

        cameraSystem.update(timeManager.getDeltaTime());

        processInput(window);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        lightingShaderProgram.use();
        lightingShaderProgram.setUniform("material.shininess", 64.0f);
        lightingShaderProgram.setUniform("light.position", lightPos);
        lightingShaderProgram.setUniform("light.ambient", ambientColor);
        lightingShaderProgram.setUniform("light.diffuse", diffuseColor);
        lightingShaderProgram.setUniform("light.specular", 1.0f, 1.0f, 1.0f);
        lightingShaderProgram.setUniform("viewPos", camera->getPosition());
        lightingShaderProgram.setUniform("objectColor", 1.0f, 0.5f, 0.31f);

        constexpr glm::mat4 illuminatedObjectModelMatrix{ 1.0f };
        const auto viewMatrix{ camera->getViewMatrix() };
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
            camera->getProjectionMatrix()
        );
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, diffuseMap);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, specularMap);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, emissiveTexture);

        glBindVertexArray(lightingVertexArrayObject);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        lightSourceShaderProgram.use();
        lightSourceShaderProgram.setUniform("lightColor", lightColor);
        const auto lightSourceModelMatrix{
            glm::scale(
                glm::translate(
                    glm::mat4{ 1.0f },
                    lightPos
                ),
                glm::vec3{ 0.2 }
            )
        };
        lightSourceShaderProgram.setUniform(
            "model",
            lightSourceModelMatrix
        );
        lightSourceShaderProgram.setUniform("view", viewMatrix);
        lightSourceShaderProgram.setUniform("projection", camera->getProjectionMatrix());
        glBindVertexArray(lightSourceVertexArrayObject);
        glDrawArrays(GL_TRIANGLES, 0, 36);

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
