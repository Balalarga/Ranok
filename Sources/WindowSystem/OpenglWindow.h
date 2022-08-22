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
    void PostRender() override;
    
    void SetVSync(bool enabled) override;

    void ClearImGui() override;
    void PostRenderImGui() override;


private:
    SDL_GLContext _glContext;
};
