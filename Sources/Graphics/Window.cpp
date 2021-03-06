#include "Window.h"

#include "Graphics/Gui/Gui.h"

#include <iostream>
#include <SDL_image.h>
#include <SDL_opengl.h>
#include <imgui_impl_sdl.h>
#include <imgui_impl_opengl3.h>


Window* Window::_self = nullptr;

Window *Window::Create(const WindowParams &params)
{
    if (!_self)
        _self = new Window(params);

    if (!_self->_sdlWindow)
    {
        delete _self;
        _self = nullptr;
    }

    return _self;
}

Window *Window::Get()
{
    return _self;
}

void Window::Destroy()
{
    if (_self)
    {
        delete _self;
        _self = nullptr;
    }
}


Window::Window(const WindowParams &params):
    _windowColor(0.2f, 0.2f, 0.2f, 1.00f)
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0)
    {
        std::cout << "Error: " << SDL_GetError() << std::endl;
        return;
    }

    if (IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG) == 0)
    {
        std::cout << "Error: " << IMG_GetError() << std::endl;
        return;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, params.openglVersion.x);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, params.openglVersion.y);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    _sdlWindow = SDL_CreateWindow(params.title.c_str(),
                                  SDL_WINDOWPOS_CENTERED,
                                  SDL_WINDOWPOS_CENTERED,
                                  params.size.x,
                                  params.size.y,
                                  params.flags);
    _glContext = SDL_GL_CreateContext(_sdlWindow);
    SDL_GL_MakeCurrent(_sdlWindow, _glContext);
    SDL_GL_SetSwapInterval(1);
    auto err = glewInit();
    if (err != GLEW_OK)
    {
        std::cout << "Error: " << glewGetErrorString(err) << std::endl;
        SDL_GL_DeleteContext(_glContext);
        SDL_DestroyWindow(_sdlWindow);
        SDL_Quit();
        _sdlWindow = nullptr;
        return;
    }

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplSDL2_InitForOpenGL(_sdlWindow, _glContext);
    ImGui_ImplOpenGL3_Init(params.glsl_version.c_str());

    ImGuiIO& io = ImGui::GetIO();
    io.WantSaveIniSettings = false;

    glEnable(GL_TEXTURE_2D);
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);
    glDepthFunc(GL_ALWAYS);
}

Window::~Window()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    IMG_Quit();

    SDL_GL_DeleteContext(_glContext);
    SDL_DestroyWindow(_sdlWindow);
    SDL_Quit();
}

void Window::Open()
{
    _isOpened = true;

    float fps = 60.f;
    float frameTime = 1000.f/fps;
    float frameStart;
    float framePassed;

    while(_isOpened)
    {
        frameStart = SDL_GetTicks();
        PollEvents();
        Update();
        Render();

        frameStart = SDL_GetTicks() - frameStart;
//        if(frameTime > framePassed)
//            SDL_Delay(frameTime - framePassed);
    }
}

void Window::Close()
{
    _isOpened = false;
}

void Window::Render()
{
    ImGuiIO& io = ImGui::GetIO();
    io.WantSaveIniSettings = false;

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    // Render gui
    for(auto& i: _guiItems)
        i->Render();

    ImGui::Render();
    glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
    glClearColor(_windowColor.x, _windowColor.y, _windowColor.z, _windowColor.w);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    SDL_GL_SwapWindow(_sdlWindow);
}

void Window::Update()
{

}

void Window::PollEvents()
{
    static SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        ImGui_ImplSDL2_ProcessEvent(&event);
        if (event.type == SDL_QUIT)
        {
            _isOpened = false;
        }
        else if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_CLOSE &&
                event.window.windowID == SDL_GetWindowID(_sdlWindow))
        {
            _isOpened = false;
        }
    }
}

void Window::AddItem(GuiBase* object)
{
    _guiItems.push_back(object);
}
