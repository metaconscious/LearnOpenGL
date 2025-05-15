#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <filesystem>
#include <fstream>
#include <print>
#include <ranges>
#include <thread>

float vertices0[]{
    //x,   y,    z
    -.5f, 0.0f, 0.0f, // 0
    0.5f, 0.0f, 0.0f, // 1
    0.0f, 0.5f, 0.0f, // 2
};

float vertices1[]{
    //x,   y,    z
    -.5f, 0.0f, 0.0f, // 0
    0.5f, 0.0f, 0.0f, // 1
    0.0f, -.5f, 0.0f, // 2
};

[[nodiscard]] std::string readAll(const std::filesystem::path& filepath)
{
    std::ifstream file{ filepath, std::ios::binary };
    if (!file.is_open())
    {
        throw std::runtime_error("Could not open file");
    }

    const auto fileSize = std::filesystem::file_size(filepath);
    if (fileSize == 0)
    {
        std::println(stderr, "Reading an empty file: {}", filepath.string());
        return {};
    }

    std::string content(fileSize, '\0');
    file.read(content.data(), static_cast<std::streamsize>(content.size()));
    return content;
}

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

void checkShaderCompilingSuccessfulness(const GLuint shader)
{
    GLint success{};
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (success == GL_FALSE)
    {
        GLchar infoLog[512];
        glGetShaderInfoLog(shader, 512, nullptr, infoLog);

        GLint shaderType{};
        glGetShaderiv(shader, GL_SHADER_TYPE, &shaderType);
        std::string shaderTypeName{};
        switch (shaderType)
        {
            case GL_VERTEX_SHADER:
                shaderTypeName = "vertex";
                break;
            case GL_FRAGMENT_SHADER:
                shaderTypeName = "fragment";
                break;
            default:
                throw std::runtime_error("Not implement yet");
        }

        std::println(stderr, "{} shader compilation failed:\n{}", shaderTypeName, infoLog);
    }
}

void checkProgramLinkageSuccessfulness(const GLuint shaderProgram)
{
    GLint success{};
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (success == GL_FALSE)
    {
        GLchar infoLog[512];
        glGetProgramInfoLog(shaderProgram, 512, nullptr, infoLog);
        std::println(stderr, "Shader program linkage failed:\n{}", infoLog);
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

    const auto vertexShaderSource{ readAll("shaders/vertex.glsl") };
    const auto* vertexShaderSourceRaw{ vertexShaderSource.c_str() };
    const auto vertexShader{ glCreateShader(GL_VERTEX_SHADER) };
    glShaderSource(vertexShader, 1, &vertexShaderSourceRaw, nullptr);
    glCompileShader(vertexShader);

    checkShaderCompilingSuccessfulness(vertexShader);

    const auto fragmentShader0Source{ readAll("shaders/fragment0.glsl") };
    const auto* fragmentShader0SourceRaw{ fragmentShader0Source.c_str() };
    const auto fragmentShader0{ glCreateShader(GL_FRAGMENT_SHADER) };
    glShaderSource(fragmentShader0, 1, &fragmentShader0SourceRaw, nullptr);
    glCompileShader(fragmentShader0);

    checkShaderCompilingSuccessfulness(fragmentShader0);

    const auto fragmentShader1Source{ readAll("shaders/fragment1.glsl") };
    const auto* fragmentShader1SourceRaw{ fragmentShader1Source.c_str() };
    const auto fragmentShader1{ glCreateShader(GL_FRAGMENT_SHADER) };
    glShaderSource(fragmentShader1, 1, &fragmentShader1SourceRaw, nullptr);
    glCompileShader(fragmentShader1);

    checkShaderCompilingSuccessfulness(fragmentShader1);

    const auto shaderProgram0{ glCreateProgram() };
    glAttachShader(shaderProgram0, vertexShader);
    glAttachShader(shaderProgram0, fragmentShader0);
    glLinkProgram(shaderProgram0);

    checkProgramLinkageSuccessfulness(shaderProgram0);

    const auto shaderProgram1{ glCreateProgram() };
    glAttachShader(shaderProgram1, vertexShader);
    glAttachShader(shaderProgram1, fragmentShader1);
    glLinkProgram(shaderProgram1);

    checkProgramLinkageSuccessfulness(shaderProgram1);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader0);
    glDeleteShader(fragmentShader1);

    GLuint vertexArrayObjects[2]{};
    glGenVertexArrays(2, vertexArrayObjects);

    GLuint vertexBufferObjects[2]{};
    glGenBuffers(2, vertexBufferObjects);

    glBindVertexArray(vertexArrayObjects[0]);

    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObjects[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices0), vertices0, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(decltype(*vertices0)), nullptr);
    glEnableVertexAttribArray(0);

    glBindVertexArray(vertexArrayObjects[1]);

    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferObjects[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices1), vertices1, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(decltype(*vertices1)), nullptr);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);

    std::uint_fast8_t flag{ 1 };

    while (!glfwWindowShouldClose(window))
    {
        processInput(window);

        glClearColor(0.2f, 0.3f, 0.4f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram0);
        glBindVertexArray(vertexArrayObjects[0]);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glUseProgram(shaderProgram1);
        glBindVertexArray(vertexArrayObjects[1]);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        // Note: double buffer is used by default for modern OpenGL
        glfwSwapBuffers(window); // Swap back buffer to front as front buffer
        glfwPollEvents(); // Processes the event queue and invoke appropriate callbacks
    }

    glDeleteVertexArrays(2, vertexArrayObjects);
    glDeleteBuffers(2, vertexBufferObjects);
    glDeleteProgram(shaderProgram0);
    glDeleteProgram(shaderProgram1);

    glfwTerminate();

    return 0;
}
