file(GLOB SHADERS "${CMAKE_CURRENT_SOURCE_DIR}/shaders/*")
add_custom_command(
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/shaders.timestamp
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/shaders
        ${CMAKE_CURRENT_BINARY_DIR}/shaders
        COMMAND ${CMAKE_COMMAND} -E touch ${CMAKE_CURRENT_BINARY_DIR}/shaders.timestamp
        DEPENDS ${SHADERS}
        COMMENT "Copying shaders..."
)
add_custom_target(copy_shaders ALL DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/shaders.timestamp)

file(GLOB_RECURSE RESOURCES "${CMAKE_CURRENT_SOURCE_DIR}/resources/*")
add_custom_command(
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/resources.timestamp
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/resources
        ${CMAKE_CURRENT_BINARY_DIR}/resources
        COMMAND ${CMAKE_COMMAND} -E touch ${CMAKE_CURRENT_BINARY_DIR}/resources.timestamp
        DEPENDS ${RESOURCES}
        COMMENT "Copying resources..."
)
add_custom_target(copy_resources ALL DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/resources.timestamp)

file(GLOB APP_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
add_library(LearnOpenGLLibrary ${APP_SOURCES})
target_include_directories(LearnOpenGLLibrary PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)
target_link_libraries(LearnOpenGLLibrary PUBLIC OpenGL::GL glfw glad glm::glm Boost::boost PNG::PNG JPEG::JPEG)

add_executable(LearnOpenGL src/main.cpp)
target_link_libraries(LearnOpenGL PRIVATE LearnOpenGLLibrary OpenGL::GL glfw glad glm::glm Boost::boost PNG::PNG JPEG::JPEG)