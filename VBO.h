// VBO.h
#ifndef VBO_H
#define VBO_H

#include <glad/glad.h>
#include <vector>
#include "CommonTypes.h"

class VBO {
public:
    GLuint ID;
    VBO(const std::vector<Vertex>& vertices);
    void Bind();
    void Unbind();
    void Delete();
};

#endif