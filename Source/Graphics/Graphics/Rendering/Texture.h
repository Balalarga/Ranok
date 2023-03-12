#pragma once

#include <GL/glew.h>
#include "glm/glm.hpp"

class Texture
{
public:
    Texture(glm::uvec2 size, int channelsMode = GL_RGB);
    virtual ~Texture();

    void Resize(glm::uvec2 size);
    void Recreate(glm::uvec2 size);

    void Bind();
    void Release();

    glm::uvec2 Size() const { return _size; }
    unsigned GetId() const { return _textureId; }


private:
    unsigned _textureId;
    int _channelsMode;
    glm::uvec2 _size;
};
