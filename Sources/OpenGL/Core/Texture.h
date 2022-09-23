#pragma once

#include <GL/glew.h>
#include "glm/glm.hpp"

namespace Ranok
{
class Texture
{
public:
    Texture(glm::uvec2 size);
    virtual ~Texture();

    unsigned Create();

    void Bind();
    void Release();

    glm::uvec2 Size() const { return _size; }
    unsigned GetId() const { return _textureId; }


private:
    unsigned _textureId;
    glm::uvec2 _size;
};
}
