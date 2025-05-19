cmake_minimum_required(VERSION 4.0.1)
project(3DModelLoader)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Add external dependencies
add_subdirectory(external/glfw-3.4)
add_subdirectory(external/glad)
add_subdirectory(external/glm)
add_subdirectory(external/tinygltf-2.9.5)

# Source files
set(SOURCES
    src/main.cpp
    src/renderer.cpp
    src/renderer.hpp
    src/model.cpp
    src/model.hpp
    src/shader.cpp
    src/shader.hpp
)

# Include directories
include_directories(
    external/glfw/include
    external/glad/include
    external/glm
    external/tinygltf
    src
)

add_executable(3DModelLoader ${SOURCES})

# Link libraries
target_link_libraries(3DModelLoader
    glfw
    glad
    glm
    tinygltf
)

# On Windows, link OpenGL
if (WIN32)
    target_link_libraries(3DModelLoader opengl32)
else()
    find_package(OpenGL REQUIRED)
    target_link_libraries(3DModelLoader ${OPENGL_LIBRARIES})
endif()