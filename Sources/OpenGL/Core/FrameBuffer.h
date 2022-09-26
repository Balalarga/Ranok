#pragma once

#include "glm/glm.hpp"
#include "OpenGL/Core/Texture.h"


namespace Ranok
{
class FrameBuffer
{
public:
    FrameBuffer(glm::uvec2 size);
    ~FrameBuffer();

    unsigned Create();

    void Bind();
    void Release();

    unsigned GetTextureId() const { return _texture.GetId(); }

    glm::uvec2 GetTextureSize() { return _texture.Size(); }

private:
    Texture _texture;
    unsigned _fboId;
};
}
