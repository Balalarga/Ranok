#ifndef WINDOW_H
#define WINDOW_H

#include <GL/glew.h>
#include <SDL.h>
#include <imgui.h>
#include <string>
#include <vector>

#include "Widgets/Opengl.h"
#include "Widgets/Gui.h"


struct WindowParams
{
    std::string title = "Ranok";
    SDL_Point size = {1280, 720};
    std::string glsl_version = "#version 330";
    SDL_Point openglVersion = {3, 3};
    SDL_WindowFlags flags = (SDL_WindowFlags)(SDL_WINDOW_OPENGL |
                                              SDL_WINDOW_RESIZABLE |
                                              SDL_WINDOW_ALLOW_HIGHDPI);
};

class Window
{
public:
    static Window* Create(const WindowParams& params = {});
    static Window* Get();
    static void Destroy();

    void Open();
    void Close();

    void AddItem(GuiItem* object);


protected:
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

    std::vector<GuiItem*> _guiItems;
};

#endif // WINDOW_H
