#pragma once

#include "glm/glm.hpp"

class Texture
{
public:
    Texture(glm::uvec2 size);
    virtual ~Texture();

    void Resize(glm::uvec2 size);
    void Recreate(glm::uvec2 size);

    void Bind();
    void Release();

    glm::uvec2 Size() const { return _size; }
    unsigned GetId() const { return _textureId; }


private:
    unsigned _textureId;
    glm::uvec2 _size;
};
