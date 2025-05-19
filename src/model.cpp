#include "model.hpp"
#include <iostream>

Model::Model() {}
Model::~Model() { cleanup(); }

bool Model::loadFromFile(const std::string& path) {
    tinygltf::Model gltfModel;
    tinygltf::TinyGLTF loader;
    std::string err, warn;

    bool ret = loader.LoadASCIIFromFile(&gltfModel, &err, &warn, path);

    if (!warn.empty()) std::cout << "GLTF Warning: " << warn << std::endl;
    if (!err.empty()) std::cerr << "GLTF Error: " << err << std::endl;
    if (!ret) {
        std::cerr << "Failed to load glTF file: " << path << std::endl;
        return false;
    }

    return loadMesh(gltfModel);
}

bool Model::loadMesh(const tinygltf::Model& model) {
    for (const auto& mesh : model.meshes) {
        for (const auto& primitive : mesh.primitives) {
            if (primitive.indices < 0) continue;

            const tinygltf::Accessor& indexAccessor = model.accessors[primitive.indices];
            const tinygltf::BufferView& indexBufferView = model.bufferViews[indexAccessor.bufferView];
            const tinygltf::Buffer& indexBuffer = model.buffers[indexBufferView.buffer];

            const tinygltf::Accessor& positionAccessor = model.accessors[primitive.attributes.find("POSITION")->second];
            const tinygltf::BufferView& positionBufferView = model.bufferViews[positionAccessor.bufferView];
            const tinygltf::Buffer& positionBuffer = model.buffers[positionBufferView.buffer];

            Mesh m;
            glGenVertexArrays(1, &m.vao);
            glGenBuffers(1, &m.vbo);
            glGenBuffers(1, &m.ebo);

            glBindVertexArray(m.vao);

            // Vertex buffer
            glBindBuffer(GL_ARRAY_BUFFER, m.vbo);
            glBufferData(GL_ARRAY_BUFFER,
                         positionBufferView.byteLength,
                         positionBuffer.data.data() + positionBufferView.byteOffset,
                         GL_STATIC_DRAW);

            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 12, (void*)0);

            // Index buffer
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m.ebo);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                         indexBufferView.byteLength,
                         indexBuffer.data.data() + indexBufferView.byteOffset,
                         GL_STATIC_DRAW);

            m.indexCount = indexAccessor.count;

            glBindVertexArray(0);

            meshes.push_back(m);
        }
    }

    return true;
}

void Model::draw() const {
    for (const auto& mesh : meshes) {
        glBindVertexArray(mesh.vao);
        glDrawElements(GL_TRIANGLES, mesh.indexCount, GL_UNSIGNED_SHORT, 0);
        glBindVertexArray(0);
    }
}

void Model::cleanup() {
    for (auto& mesh : meshes) {
        glDeleteBuffers(1, &mesh.vbo);
        glDeleteBuffers(1, &mesh.ebo);
        glDeleteVertexArrays(1, &mesh.vao);
    }
    meshes.clear();
}
