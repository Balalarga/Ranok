#include"OpenglWindow.h"

#include <GL/glew.h>

#include "OpenGL/Core/IRenderable.h"
#include "OpenGL/Core/Scene.h"

#if USE_IMGUI
    #include <imgui_impl_sdl.h>
    #include <imgui_impl_opengl3.h>
#endif


OpenglWindow::OpenglWindow(const ISdlWindowParams& params):
    ISdlWindow(params)
{
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

    _glContext = SDL_GL_CreateContext(GetSdlWindow());

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK)
        return;

    SDL_GL_MakeCurrent(GetSdlWindow(), _glContext);
    auto backColor = GetBackgroundColor();
    glClearColor(backColor.r, backColor.g, backColor.b, backColor.a);

#if USE_IMGUI
    const char* glsl_version = "#version 330";

    ImGui_ImplSDL2_InitForOpenGL(GetSdlWindow(), _glContext);
    ImGui_ImplOpenGL3_Init(glsl_version);
#endif
}

OpenglWindow::~OpenglWindow()
{
    SDL_GL_DeleteContext(_glContext);
#if USE_IMGUI
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
#endif
}


void OpenglWindow::SetBackgroundColor(const glm::vec4& newColor)
{
    ISdlWindow::SetBackgroundColor(newColor);
    glClearColor(newColor.r, newColor.g, newColor.b, newColor.a);
}

void OpenglWindow::Render()
{
    if (Scene* scene = GetScene())
    {
        for (auto& Obj : scene->GetShaders())
        {
            Obj->Bind();
            Obj->BatchRender();
            Obj->Unbind();
        }
    }
    ISdlWindow::Render();
}

void OpenglWindow::SetVSync(bool enabled)
{
    if (enabled)
        SDL_GL_SetSwapInterval(1);
    else
        SDL_GL_SetSwapInterval(0);
    
    ISdlWindow::SetVSync(enabled);
}

#if USE_IMGUI
void OpenglWindow::ClearImGui()
{
    ImGui_ImplOpenGL3_NewFrame();
    ISdlWindow::ClearImGui();
}

void OpenglWindow::PostRenderImGui()
{
    ISdlWindow::PostRenderImGui();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
#endif
