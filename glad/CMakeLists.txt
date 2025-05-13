enable_language(C)

# Library source files
file(GLOB_RECURSE
        GLAD_SOURCES
        src/*.c
)

# Create library
add_library(glad ${GLAD_SOURCES})
target_include_directories(glad PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)