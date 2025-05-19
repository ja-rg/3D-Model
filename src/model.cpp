#include "model.hpp"
#include "shader.hpp"
#include <iostream>
#include <filesystem>

Model::Model() {}
Model::~Model() { cleanup(); }

bool Model::loadFromFile(const std::string &path)
{
    tinygltf::Model gltfModel;
    tinygltf::TinyGLTF loader;
    std::string err, warn;

    bool ret = loader.LoadBinaryFromFile(&gltfModel, &err, &warn, path);

    if (!warn.empty())
        std::cout << "GLTF Warning: " << warn << std::endl;
    if (!err.empty())
        std::cerr << "GLTF Error: " << err << std::endl;
    if (!ret)
    {
        std::cerr << "Failed to load glTF file: " << path << std::endl;
        return false;
    }

    return loadMesh(gltfModel);
}

bool Model::loadMesh(const tinygltf::Model &model)
{

    for (const auto &mesh : model.meshes)
    {
        for (const auto &primitive : mesh.primitives)
        {
            if (primitive.indices < 0)
                continue;

            const tinygltf::Accessor &indexAccessor = model.accessors[primitive.indices];
            const tinygltf::BufferView &indexBufferView = model.bufferViews[indexAccessor.bufferView];
            const tinygltf::Buffer &indexBuffer = model.buffers[indexBufferView.buffer];

            const tinygltf::Accessor &positionAccessor = model.accessors[primitive.attributes.find("POSITION")->second];
            const tinygltf::BufferView &positionBufferView = model.bufferViews[positionAccessor.bufferView];
            const tinygltf::Buffer &positionBuffer = model.buffers[positionBufferView.buffer];

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
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 12, (void *)0);

            if (primitive.attributes.find("TEXCOORD_0") != primitive.attributes.end())
            {
                const auto &uvAccessor = model.accessors[primitive.attributes.at("TEXCOORD_0")];
                const auto &uvView = model.bufferViews[uvAccessor.bufferView];
                const auto &uvBuffer = model.buffers[uvView.buffer];

                GLuint uvVBO;
                glGenBuffers(1, &uvVBO);
                glBindBuffer(GL_ARRAY_BUFFER, uvVBO);
                glBufferData(GL_ARRAY_BUFFER, uvView.byteLength,
                             uvBuffer.data.data() + uvView.byteOffset,
                             GL_STATIC_DRAW);

                glEnableVertexAttribArray(1); // TEXCOORD location = 1
                glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *)0);
            }

            // Index buffer
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m.ebo);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                         indexBufferView.byteLength,
                         indexBuffer.data.data() + indexBufferView.byteOffset,
                         GL_STATIC_DRAW);

            m.indexCount = indexAccessor.count;

            glBindVertexArray(0);

            meshes.push_back(m);

            if (primitive.material >= 0 && primitive.material < model.materials.size())
            {
                const auto &material = model.materials[primitive.material];
                if (material.pbrMetallicRoughness.baseColorTexture.index >= 0)
                {
                    int texIndex = material.pbrMetallicRoughness.baseColorTexture.index;
                    const auto &tex = model.textures[texIndex];
                    const auto &image = model.images[tex.source];

                    GLuint texID;
                    glGenTextures(1, &texID);
                    glBindTexture(GL_TEXTURE_2D, texID);
                    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
                                 image.width, image.height, 0,
                                 GL_RGBA, GL_UNSIGNED_BYTE, image.image.data());
                    glGenerateMipmap(GL_TEXTURE_2D);

                    // Configuración básica
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
                    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

                    m.textureID = texID;
                    m.hasTexture = true;
                }
            }
        }
    }

    return true;
}

void Model::draw(const Shader &shader) const
{
    shader.use();
    for (const auto &mesh : meshes)
    {
        if (mesh.textureID)
        {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, mesh.textureID);
            shader.setInt("texSampler", 0); // asegúrate que coincida con el nombre en el shader
        }
        glBindVertexArray(mesh.vao);
        glDrawElements(GL_TRIANGLES, mesh.indexCount, GL_UNSIGNED_SHORT, 0);
        glBindVertexArray(0);
    }
}

void Model::cleanup()
{
    for (auto &mesh : meshes)
    {
        glDeleteBuffers(1, &mesh.vbo);
        glDeleteBuffers(1, &mesh.ebo);
        glDeleteVertexArrays(1, &mesh.vao);
    }
    meshes.clear();
}
