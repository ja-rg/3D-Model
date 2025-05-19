#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

class Renderer {
public:
    Renderer();
    ~Renderer();

    bool initialize();
    void render();
    void cleanup();

private:
    unsigned int VAO, VBO;
    void setupTriangle();
};
