#ifndef COMMON_TYPES_H
#define COMMON_TYPES_H

#include <glad/glad.h>
#include<glm/glm.hpp>
#include <string>

// Estructura para vértices
struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 TexCoords;
};

// Estructura para texturas
struct Texture {
    GLuint ID;
    std::string type;
    std::string path;
};

#endif

