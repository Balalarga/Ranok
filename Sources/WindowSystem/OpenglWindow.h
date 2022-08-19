#pragma once

#include "WindowSystem/ISdlWindow.h"

class Scene;


class OpenglWindow: public ISdlWindow
{
public:
    OpenglWindow(const ISdlWindowParams& params = ISdlWindowParams());
    ~OpenglWindow() override;


    void SetBackgroundColor(const glm::vec4& newColor) override;
    void Render() override;
    
    void SetVSync(bool enabled) override;

#if USE_IMGUI
    void ClearImGui() override;
    void PostRenderImGui() override;
#endif


private:
    SDL_GLContext _glContext;
};
