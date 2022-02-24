#include "Window.h"
#include <iostream>


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

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame();
    ImGui::NewFrame();

    // Render gui
    for(auto& i: _guiItems)
        i.Render();

    ImGui::Render();
    glViewport(0, 0, (int)io.DisplaySize.x, (int)io.DisplaySize.y);
    glClearColor(_windowColor.x, _windowColor.y, _windowColor.z, _windowColor.w);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Render opengl objects
    for(auto& i: _renderableObjects)
        i->Render();

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

Window::Window(const WindowParams &params):
    _windowColor(0.45f, 0.55f, 0.60f, 1.00f)
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER | SDL_INIT_GAMECONTROLLER) != 0)
    {
        std::cout << "Error: " << SDL_GetError() << std::endl;
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

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGui_ImplSDL2_InitForOpenGL(_sdlWindow, _glContext);
    ImGui_ImplOpenGL3_Init(params.glsl_version.c_str());
}

Window::~Window()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_GL_DeleteContext(_glContext);
    SDL_DestroyWindow(_sdlWindow);
    SDL_Quit();
}
