#ifndef ISdlWindow_H
#define ISdlWindow_H

#include <string>

#include <SDL.h>
#include <glm/glm.hpp>

#include "GuiLayer.h"
#include "InputSystem.h"

struct ISdlWindowParams
{
    ISdlWindowParams(const std::string& title = "New Window"):
        title(title)
    {}
    std::string title;
    unsigned x = SDL_WINDOWPOS_CENTERED;
    unsigned y = SDL_WINDOWPOS_CENTERED;
    unsigned width = 800;
    unsigned height = 600;
    unsigned flags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI;

    bool vsync = false;
    bool fullScreen = false;
};


class ISdlWindow
{
public:
    ISdlWindow(const ISdlWindowParams& params = ISdlWindowParams());
    virtual ~ISdlWindow();
    
    void AddGuiLayer(GuiLayer layer);
    
    void Show();
    void Close();

    const glm::vec4& GetBackgroundColor() { return _backColor; }
    virtual void SetBackgroundColor(const glm::vec4& newColor);
    
    virtual void HandleEvents(SDL_Event& event);
    virtual void Clear();
    virtual void Render() = 0;
    virtual void PostRender();

    virtual void SetVSync(bool enabled);
    bool GetVSync() const { return _params.vsync; }

    virtual void ClearImGui();
    virtual void RenderImGui();
    virtual void PostRenderImGui();

    SDL_Window* GetSdlWindow() const { return _sdlWindow; }
    const ISdlWindowParams& GetParams() const { return _params; }


private:
    ISdlWindowParams _params;
    SDL_Window* _sdlWindow;

    bool _bShouldClose;
    glm::vec4 _backColor;
    std::vector<GuiLayer> _guiLayers;
};


#endif //WINDOW_H
