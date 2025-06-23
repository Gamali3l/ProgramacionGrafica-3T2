#ifndef TEXTURE_H
#define TEXTURE_H


#include <glad/glad.h>
#include <string>

class Texture {
public:
    GLuint ID;  // Cambiado de 'id' a 'ID' para consistencia
    std::string type;
    std::string path;

    // Constructor explícito
    Texture(const std::string& path, const std::string& type = "texture_diffuse");

    // Métodos
    void Bind(GLenum textureUnit = GL_TEXTURE0) const;
    static void Unbind();
    void Delete();

private:
    void GenerateTexture(unsigned char* data, int width, int height, int nrComponents);
};
#endif
