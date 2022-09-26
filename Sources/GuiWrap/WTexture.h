#pragma once

#include "IWidget.h"

#include "OpenGL/Core/FrameBuffer.h"

namespace Ranok
{
class WTexture: public IWidget
{
public:
    // Mb pass framebuffer ref by ctor?
    WTexture(glm::uvec2 size):
        _frameBuffer(size)
    {
        _frameBuffer.Create();
    }

    void Render() override
    {
        ImGui::Image((void*)(intptr_t)_frameBuffer.GetTextureId(),
                     ImVec2(_frameBuffer.GetTextureSize().x, _frameBuffer.GetTextureSize().y),
                     ImVec2(0, 1),
                     ImVec2(1, 0));
    }

    FrameBuffer& GetFrameBuffer() { return _frameBuffer; }

private:
    FrameBuffer _frameBuffer;
};
}
