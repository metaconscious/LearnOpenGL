cmake_minimum_required(VERSION 4.0)
project(LearnOpenGL LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(OpenGL REQUIRED)
find_package(glfw3 3.3 REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(Boost REQUIRED)
find_package(PNG REQUIRED)
find_package(JPEG REQUIRED)
find_package(assimp REQUIRED)

add_subdirectory(glad)
add_subdirectory(app)