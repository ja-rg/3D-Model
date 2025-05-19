#pragma once

#include <string>
#include <vector>
#include <glad/glad.h>
#include <tiny_gltf.h>
#include "shader.hpp"

class Model {
public:
    Model();
    ~Model();

    bool loadFromFile(const std::string& path);
    void draw(const Shader& shader) const;
    void cleanup();

private:
    struct Mesh {
        GLuint vao = 0;
        GLuint vbo = 0;
        GLuint ebo = 0;
        GLuint textureID = 0;
        size_t indexCount = 0;

        bool hasTexture = false;
    };

    std::vector<Mesh> meshes;

    bool loadMesh(const tinygltf::Model& model);
};
