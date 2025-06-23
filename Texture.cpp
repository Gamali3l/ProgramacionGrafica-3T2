#include "Texture.h"
#include<stb_image.h>
#include <iostream>

Texture::Texture(const std::string& path, const std::string& type)
    : ID(0), type(type), path(path)  // Inicialización explícita de ID
{
    stbi_set_flip_vertically_on_load(true);

    int width, height, nrComponents;
    unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrComponents, 0);

    if (data) {
        GenerateTexture(data, width, height, nrComponents);
        stbi_image_free(data);
    }
    else {
        std::cerr << "ERROR::TEXTURE: Failed to load texture at path: " << path << std::endl;
        stbi_image_free(data);
        // Crear textura de fallback (1x1 píxel blanco)
        unsigned char fallbackData[] = { 255, 255, 255, 255 };
        GenerateTexture(fallbackData, 1, 1, 4);
    }
}

void Texture::GenerateTexture(unsigned char* data, int width, int height, int nrComponents) {
    glGenTextures(1, &ID);
    glBindTexture(GL_TEXTURE_2D, ID);
    GLfloat aniso = 0.0f;
    glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY, &aniso);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY, aniso);

    GLenum format = GL_RGB;
    if (nrComponents == 1)
        format = GL_RED;
    else if (nrComponents == 3)
        format = GL_RGB;
    else if (nrComponents == 4)
        format = GL_RGBA;

    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::Bind(GLenum textureUnit) const {
    glActiveTexture(textureUnit);
    glBindTexture(GL_TEXTURE_2D, ID);
}

void Texture::Unbind() {
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::Delete() {
    glDeleteTextures(1, &ID);
}