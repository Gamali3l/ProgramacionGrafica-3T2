// EBO.h
#ifndef EBO_H
#define EBO_H

#include <glad/glad.h>
#include <vector>

class EBO {
public:
    GLuint ID;
    EBO(const std::vector<unsigned int>& indices);
    void Bind();
    void Unbind();
    void Delete();
};

#endif