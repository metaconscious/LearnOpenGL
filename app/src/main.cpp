#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <cmath>
#include <filesystem>
#include <fstream>
#include <print>
#include <ranges>

#include "../include/app/Shader.h"

float verticesWithColors[] = {
    // positions         // colors
    0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom right
    -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, // bottom left
    0.0f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f // top
};

void setViewportWithFramebufferSize([[maybe_unused]] GLFWwindow* window,
                                    const int width,
                                    const int height)
{
    glViewport(0, 0, width, height);
}

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

    auto* window{ glfwCreateWindow(800, 600, "LearnOpenGL", nullptr, nullptr) };
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

    const auto shader{ lgl::Shader::LoadShader("shaders/vertex.glsl", "shaders/fragment.glsl") };

    GLuint vertexArrayObject{};
    glGenVertexArrays(1, &vertexArrayObject);

    GLuint vertexBufferObject{};
    glGenBuffers(1, &vertexBufferObject);

    glBindVertexArray(vertexArrayObject);

    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObject);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticesWithColors), verticesWithColors, GL_STATIC_DRAW);

    glVertexAttribPointer(0,
                          3,
                          GL_FLOAT,
                          GL_FALSE,
                          6 * sizeof(std::ranges::range_value_t<decltype(verticesWithColors)>),
                          nullptr);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1,
                          3,
                          GL_FLOAT,
                          GL_FALSE,
                          6 * sizeof(std::ranges::range_value_t<decltype(verticesWithColors)>),
                          reinterpret_cast<void*>(sizeof(std::ranges::range_value_t<decltype(verticesWithColors)>) *
                              3));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0); // Optional

    glBindVertexArray(0); // Optional. DO NOT unbind EBO above this line or VAO will remember "NO EBO".

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); // Since VAO is already unbounded, it's safe to unbind EBO now.

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        glClearColor(0.2f, 0.3f, 0.4f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        shader.use();
        glBindVertexArray(vertexArrayObject);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        // Note: double buffer is used by default for modern OpenGL
        glfwSwapBuffers(window); // Swap back buffer to front as front buffer
        glfwPollEvents(); // Processes the event queue and invoke appropriate callbacks
    }

    glDeleteVertexArrays(1, &vertexArrayObject);
    glDeleteBuffers(1, &vertexBufferObject);

    glfwTerminate();

    return 0;
}
