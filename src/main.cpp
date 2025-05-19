#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include "shader.hpp"
#include "renderer.hpp"
#include "model.hpp"

int main()
{
    if (!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW\n";
        return -1;
    }

    GLFWwindow *window = glfwCreateWindow(640, 480, "Renderer Example", nullptr, nullptr);
    if (!window)
    {
        std::cerr << "Failed to create window\n";
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cerr << "Failed to initialize GLAD\n";
        return -1;
    }

    Model model;
    if (!model.loadFromFile("models/BarramundiFish.glb"))
    {
        return -1;
    }

    Shader shader("shaders/vertex_shader.glsl", "shaders/fragment_shader.glsl");


    while (!glfwWindowShouldClose(window))
    {
        // Clear the screen
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        model.draw(shader);
        
        // Draw the model
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    model.cleanup();

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
