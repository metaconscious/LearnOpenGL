#include "app/Image.h"
#include "app/Shader.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>

#include <cmath>
#include <filesystem>
#include <fstream>
#include <print>
#include <ranges>

constexpr auto INITIAL_WINDOW_WIDTH{ 800 };
constexpr auto INITIAL_WINDOW_HEIGHT{ 600 };

int windowWidth{ INITIAL_WINDOW_WIDTH };
int windowHeight{ INITIAL_WINDOW_HEIGHT };

float vertices[] = {
    -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,
    0.5f, -0.5f, -0.5f, 1.0f, 0.0f,
    0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
    0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
    -0.5f, 0.5f, -0.5f, 0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,

    -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
    0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
    0.5f, 0.5f, 0.5f, 1.0f, 1.0f,
    0.5f, 0.5f, 0.5f, 1.0f, 1.0f,
    -0.5f, 0.5f, 0.5f, 0.0f, 1.0f,
    -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,

    -0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
    -0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
    -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
    -0.5f, 0.5f, 0.5f, 1.0f, 0.0f,

    0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
    0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
    0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
    0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
    0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
    0.5f, 0.5f, 0.5f, 1.0f, 0.0f,

    -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
    0.5f, -0.5f, -0.5f, 1.0f, 1.0f,
    0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
    0.5f, -0.5f, 0.5f, 1.0f, 0.0f,
    -0.5f, -0.5f, 0.5f, 0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,

    -0.5f, 0.5f, -0.5f, 0.0f, 1.0f,
    0.5f, 0.5f, -0.5f, 1.0f, 1.0f,
    0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
    0.5f, 0.5f, 0.5f, 1.0f, 0.0f,
    -0.5f, 0.5f, 0.5f, 0.0f, 0.0f,
    -0.5f, 0.5f, -0.5f, 0.0f, 1.0f
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

bool equalPressing{ false };
bool minusPressing{ false };
float fieldOfView{ 45.0f };

void setViewportWithFramebufferSize([[maybe_unused]] GLFWwindow* window,
                                    const int width,
                                    const int height)
{
    windowWidth = width;
    windowHeight = height;
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
    if (glfwGetKey(window, GLFW_KEY_EQUAL) == GLFW_PRESS)
    {
        equalPressing = true;
        fieldOfView += 0.1f;
    }
    else if (equalPressing && glfwGetKey(window, GLFW_KEY_EQUAL) == GLFW_RELEASE)
    {
        equalPressing = false;
        std::println("Current FOV is {}", fieldOfView);
    }
    if (glfwGetKey(window, GLFW_KEY_MINUS) == GLFW_PRESS)
    {
        minusPressing = true;
        fieldOfView -= 0.1f;
    }
    else if (minusPressing && glfwGetKey(window, GLFW_KEY_MINUS) == GLFW_RELEASE)
    {
        minusPressing = false;
        std::println("Current FOV is {}", fieldOfView);
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
            INITIAL_WINDOW_WIDTH,
            INITIAL_WINDOW_HEIGHT,
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

    glfwSetFramebufferSizeCallback(window, &setViewportWithFramebufferSize);

    glEnable(GL_DEPTH_TEST);

    const auto shader{ lgl::Shader::load("shaders/vertex.glsl", "shaders/fragment.glsl") };

    GLuint vertexArrayObject{};
    glGenVertexArrays(1, &vertexArrayObject);

    GLuint vertexBufferObject{};
    glGenBuffers(1, &vertexBufferObject);

    GLuint elementBufferObject{};
    glGenBuffers(1, &elementBufferObject);

    glBindVertexArray(vertexArrayObject);

    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0,
                          3,
                          GL_FLOAT,
                          GL_FALSE,
                          5 * sizeof(std::ranges::range_value_t<decltype(vertices)>),
                          nullptr);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1,
                          2,
                          GL_FLOAT,
                          GL_FALSE,
                          5 * sizeof(std::ranges::range_value_t<decltype(vertices)>),
                          reinterpret_cast<void*>(sizeof(std::ranges::range_value_t<decltype(vertices)>) * 3));
    glEnableVertexAttribArray(1);

    const auto textureImage0{ lgl::loadImageAsTexture("resources/textures/container.jpg").flipVertically() };
    const auto textureImage1{ lgl::loadImageAsTexture("resources/textures/awesomeface.png").flipVertically() };

    GLuint texture0{};
    glGenTextures(1, &texture0);

    GLuint texture1{};
    glGenTextures(1, &texture1);

    glBindTexture(GL_TEXTURE_2D, texture0);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 GL_RGB,
                 static_cast<GLsizei>(textureImage0.width),
                 static_cast<GLsizei>(textureImage0.height),
                 0,
                 GL_RGB,
                 GL_UNSIGNED_BYTE,
                 textureImage0.span().data());
    glGenerateMipmap(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, texture1);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 GL_RGBA,
                 static_cast<GLsizei>(textureImage1.width),
                 static_cast<GLsizei>(textureImage1.height),
                 0,
                 GL_RGBA,
                 GL_UNSIGNED_BYTE,
                 textureImage1.span().data());
    glGenerateMipmap(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, 0);

    glBindBuffer(GL_ARRAY_BUFFER, 0); // Optional

    glBindVertexArray(0); // Optional. DO NOT unbind EBO above this line or VAO will remember "NO EBO".

    shader.use();
    shader.setUniform("texture0", 0);
    shader.setUniform("texture1", 1);

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        glClearColor(0.2f, 0.3f, 0.4f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture0);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture1);

        shader.use();
        shader.setUniform(
            "view",
            glm::translate(
                glm::mat4(1.0f),
                glm::vec3(0.0f, 0.0f, -3.0f)
            )
        );
        glBindVertexArray(vertexArrayObject);
        for (auto&& [index, vec] : std::views::enumerate(cubePositions))
        {
            shader.setUniform(
                "model",
                glm::rotate(
                    glm::translate(
                        glm::mat4(1.0f),
                        vec
                    ),
                    static_cast<float>(glfwGetTime()) * glm::radians(20.0f * static_cast<float>(index)),
                    glm::vec3(1.0f, 0.3f, 0.5f)
                )
            );
            shader.setUniform(
                "projection",
                glm::perspective(
                    glm::radians(fieldOfView),
                    static_cast<float>(windowWidth) / static_cast<float>(windowHeight),
                    0.1f,
                    100.0f
                )
            );
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        // Note: double buffer is used by default for modern OpenGL
        glfwSwapBuffers(window); // Swap back buffer to front as front buffer
        glfwPollEvents(); // Processes the event queue and invoke appropriate callbacks
    }

    glDeleteVertexArrays(1, &vertexArrayObject);
    glDeleteBuffers(1, &vertexBufferObject);
    glDeleteBuffers(1, &elementBufferObject);

    glfwTerminate();

    return 0;
}
