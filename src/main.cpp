#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include "shader.hpp"
#include "renderer.hpp"
#include "model.hpp"

#ifndef PRODUCTION
#define BASE_ROOT "../../"
#else
#define BASE_ROOT "./"
#endif

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
    if (!model.loadFromFile(BASE_ROOT "models/BarramundiFish.glb"))
    {
        return -1;
    }

    Shader shader(BASE_ROOT "src/shaders/vertex_shader.glsl", BASE_ROOT "src/shaders/fragment_shader.glsl");

    while (!glfwWindowShouldClose(window))
    {
        // Clear the screen
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Set color
        shader.setFloat("color", 1.0f, 0.5f, 0.2f);

        // Set the view and projection matrices
        // View - Move camera backward only
        glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 0.2f, -1.0f), // Camera position
                                     glm::vec3(0.0f, 0.0f, 0.0f),  // Look at the origin
                                     glm::vec3(0.0f, 1.0f, 0.0f)); // Up vector

        // Projection - unchanged
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), 640.0f / 480.0f, 0.1f, 100.0f);

        // Model - rotate around Y axis over time
        glm::mat4 modelMatrix = glm::mat4(1.0f);
        modelMatrix = glm::rotate(modelMatrix, (float)glfwGetTime(), glm::vec3(0.0f, 1.0f, 0.0f));

        shader.setMat4("view", view);
        shader.setMat4("projection", projection);
        shader.setMat4("model", modelMatrix);

        glm::vec3 lightPos(1.0f, 1.0f, 1.0f);
        shader.setVec3("lightPos", lightPos);

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
