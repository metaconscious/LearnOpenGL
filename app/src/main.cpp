#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <print>

void setViewportWithFramebufferSize([[maybe_unused]] GLFWwindow* window,
                                    const int width,
                                    const int height)
{
    glViewport(0, 0, width, height);
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

    while (!glfwWindowShouldClose(window))
    {
        // Note: double buffer is used by default for modern OpenGL
        glfwSwapBuffers(window); // Swap back buffer to front as front buffer
        glfwPollEvents(); // Processes the event queue and invoke appropriate callbacks
    }

    glfwTerminate();

    return 0;
}
