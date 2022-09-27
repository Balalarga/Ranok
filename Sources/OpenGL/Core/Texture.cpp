#include "Texture.h"
#include "OpenGL/ErrorHandle.h"

namespace Ranok
{
Texture::Texture(glm::uvec2 size):
    _size(size)
{
}

Texture::~Texture()
{
    glDeleteTextures(1, &_textureId);
}

void Texture::Resize(glm::uvec2 size)
{
    if (_size == size)
        return;
    
    _size = size;
    glBindTexture(GL_TEXTURE_2D, _textureId);
    GLCall(glTexImage2D(GL_TEXTURE_2D,
                 0,
                 GL_RGB,
                 _size.x,
                 _size.y,
                 0,
                 GL_RGB,
                 GL_UNSIGNED_BYTE,
                 NULL))
    
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR))
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR))
    
    glBindTexture(GL_TEXTURE_2D, 0);
}

unsigned Texture::Create()
{
    GLCall(glGenTextures(1, &_textureId))
    GLCall(glBindTexture(GL_TEXTURE_2D, _textureId))

    GLCall(glTexImage2D(GL_TEXTURE_2D,
                 0,
                 GL_RGB,
                 _size.x,
                 _size.y,
                 0,
                 GL_RGB,
                 GL_UNSIGNED_BYTE,
                 NULL))

    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR))
    GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR))
    GLCall(glBindTexture(GL_TEXTURE_2D, 0))
    return _textureId;
}

void Texture::Bind()
{
    glBindTexture(GL_TEXTURE_2D, _textureId);
}

void Texture::Release()
{
    glBindTexture(GL_TEXTURE_2D, 0);
}
}
