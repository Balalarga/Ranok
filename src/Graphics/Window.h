#ifndef WINDOW_H
#define WINDOW_H

#include <SDL.h>
#include <SDL_opengl.h>
#include <imgui.h>
#include <imgui_impl_sdl.h>
#include <imgui_impl_opengl3.h>
#include <string>
#include <vector>

#include "Renderable.h"
#include "GuiItem.h"


struct WindowParams
{
    std::string title = "Ranok";
    SDL_Point size = {1280, 720};
    std::string glsl_version = "#version 330";
    SDL_Point openglVersion = {3, 3};
    SDL_WindowFlags flags = (SDL_WindowFlags)(SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
};

class Window
{
public:
    static Window* Create(const WindowParams& params = {});
    static Window* Get();
    static void Destroy();

    void Open();
    void Close();

    void Render();
    void Update();
    void PollEvents();


private:
    Window(const WindowParams& params);
    ~Window();
    static Window* _self;

    SDL_Window* _sdlWindow = nullptr;
    SDL_GLContext _glContext;
    bool _isOpened = false;

    ImVec4 _windowColor;

    std::vector<Renderable*> _renderableObjects;
    std::vector<GuiItem> _guiItems;
};

#endif // WINDOW_H
