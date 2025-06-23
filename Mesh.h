// Mesh.h
#ifndef MESH_H
#define MESH_H

#include <vector>
#include "CommonTypes.h"
#include "VAO.h"
#include "VBO.h"
#include "EBO.h"
#include "Shaderclass.h"

class Mesh {
public:
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;


    VAO vao;
    VBO* vbo;
    EBO* ebo;

    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);
    void Draw(Shader& shader);
    void Delete();

private:
    void setupMesh();
};

#endif