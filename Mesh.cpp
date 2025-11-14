// Mesh.cpp
#include "Mesh.h"

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures)
    : vertices(vertices), indices(indices), textures(textures)
{
    setupMesh();
}

void Mesh::setupMesh()
{
    vao.Bind();
    vbo = new VBO(vertices);
    ebo = new EBO(indices);

    vao.LinkAttrib(*vbo, 0, 3, GL_FLOAT, sizeof(Vertex), (void*)offsetof(Vertex, Position));
    vao.LinkAttrib(*vbo, 1, 3, GL_FLOAT, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
    vao.LinkAttrib(*vbo, 2, 2, GL_FLOAT, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));

    vao.Unbind();
    vbo->Unbind();
    ebo->Unbind();
}


void Mesh::Draw(Shader& shader)
{
    unsigned int diffuseNr = 1;
    unsigned int specularNr = 1;
    unsigned int basecolorNr = 1;
    unsigned int normalNr = 1;
    unsigned int roughnessNr = 1;
    unsigned int metalnessNr = 1;

    for (unsigned int i = 0; i < textures.size(); i++) {
        glActiveTexture(GL_TEXTURE0 + i);

        std::string number;
        std::string name = textures[i].type;

        if (name == "texture_diffuse" || name == "texture_basecolor") number = std::to_string(diffuseNr++);
        else if (name == "texture_specular") number = std::to_string(specularNr++);
        else if (name == "texture_normal") number = std::to_string(normalNr++);
        else if (name == "texture_roughness") number = std::to_string(roughnessNr++);
        else if (name == "texture_metalness") number = std::to_string(metalnessNr++);

        shader.setInt((name + number).c_str(), i);
        glBindTexture(GL_TEXTURE_2D, textures[i].ID);
    }

    vao.Bind();
    glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);
    vao.Unbind();
    glActiveTexture(GL_TEXTURE0);
}

void Mesh::Delete()
{
    vao.Delete();
    vbo->Delete();
    ebo->Delete();
    delete vbo;
    delete ebo;
}