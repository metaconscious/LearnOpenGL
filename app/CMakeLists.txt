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

add_executable(LearnOpenGL src/main.cpp)
target_link_libraries(LearnOpenGL OpenGL::GL glfw glad)