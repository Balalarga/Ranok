#include "Scene.h"
#include "Graphics/Opengl/Renderable.h"
#include "Ranok/Utility/Math.h"

#include <functional>
#include <SDL.h>
#include "Graphics/Window.h"


Scene::Scene(ImVec2 renderSize, const std::string& title):
    GuiBase(title),
    _renderSize(renderSize),
    _backgroundColor({0.5, 0.5, 0.5, 1.0}),
    _needUpdate(true),
    _mouseHandle(false)
{
    glGenFramebuffers(1, &_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, _fbo);

    glGenTextures(1, &_texture);
    glBindTexture(GL_TEXTURE_2D, _texture);

    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 GL_RGB,
                 _renderSize.x,
                 _renderSize.y,
                 0,
                 GL_RGB,
                 GL_UNSIGNED_BYTE,
                 NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindTexture(GL_TEXTURE_2D, 0);

    glFramebufferTexture2D(GL_FRAMEBUFFER,
                           GL_COLOR_ATTACHMENT0,
                           GL_TEXTURE_2D,
                           _texture,
                           0);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    _camera.SetSize({renderSize.x, renderSize.y});
}

Scene::~Scene()
{
    glDeleteTextures(1, &_texture);
    glDeleteFramebuffers(1, &_fbo);
}

void Scene::Render()
{
    if (_needUpdate)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
        glViewport(0, 0, _renderSize.x, _renderSize.y);
        glClearColor(_backgroundColor.x,
                     _backgroundColor.y,
                     _backgroundColor.z,
                     _backgroundColor.w);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        for(auto& i: _objects)
            i->Render();

        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        _needUpdate = false;
    }


    ImGuiIO& io = ImGui::GetIO();
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 0.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 0);
    ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
    ImGui::BeginChild(Name().c_str(), Size());

    ImVec2 mousePosAbs = ImGui::GetMousePos();
    ImVec2 windowPosAbs = ImGui::GetItemRectMin();
    ImVec2 mousePosRel = ImVec2(mousePosAbs.x - windowPosAbs.x,
                                mousePosAbs.y - windowPosAbs.y);
    ImVec2 windowSize = ImGui::GetWindowSize();

    bool isMouseInside = mousePosRel.x > 0 && mousePosRel.y > 0 &&
            mousePosRel.x < windowSize.x && mousePosRel.y < windowSize.y;

    if (ImGui::IsMouseDown(ImGuiMouseButton_Right) && (isMouseInside || _mouseHandle))
    {
        _mouseHandle = true;
        HandleMouse(io.MouseDelta);
        HandleScroll(io.MouseWheel);
    }
    else if (isMouseInside)
    {
        HandleScroll(io.MouseWheel);
    }

    if (!ImGui::IsMouseDown(ImGuiMouseButton_Right))
    {
        _mouseHandle = false;
    }


    const Uint8 *state = SDL_GetKeyboardState(NULL);
    if (state[SDL_SCANCODE_W] || state[SDL_SCANCODE_UP])
        HandleKeyboard(Camera::Camera_Movement::FORWARD, 0.1);
    else if (state[SDL_SCANCODE_S] || state[SDL_SCANCODE_DOWN])
        HandleKeyboard(Camera::Camera_Movement::BACKWARD, 0.1);
    else if (state[SDL_SCANCODE_A] || state[SDL_SCANCODE_LEFT])
        HandleKeyboard(Camera::Camera_Movement::LEFT, 0.1);
    else if (state[SDL_SCANCODE_D] || state[SDL_SCANCODE_RIGHT])
        HandleKeyboard(Camera::Camera_Movement::RIGHT, 0.1);


    ImGui::Image((void*)(intptr_t)_texture, ImGui::GetWindowSize(), ImVec2(0, 1), ImVec2(1, 0));

    ImGui::EndChild();
    ImGui::PopStyleVar(4);
}

void Scene::SetBackgroundColor(const ImVec4 &color)
{
    _backgroundColor = color;
}

void Scene::SetRenderSize(unsigned x, unsigned y)
{
    if (_renderSize.x != x ||
        _renderSize.y != y)
    {
        _renderSize.x = x;
        _renderSize.y = y;
        UpdateTexture();
        _camera.SetSize({x, y});
    }
}

void Scene::HandleMouse(const ImVec2 &mouseDelta)
{
    if (!Math::IsZero(mouseDelta.x) && !Math::IsZero(mouseDelta.y))
    {
        _camera.ProcessMouseMovement(mouseDelta.x, -mouseDelta.y);
        _needUpdate = true;
    }
}

void Scene::HandleKeyboard(Camera::Camera_Movement dir, float deltaTime)
{
    _camera.ProcessKeyboard(dir, deltaTime);
    _needUpdate = true;
}

void Scene::HandleScroll(float delta)
{
    if (!Math::IsZero(delta))
    {
        _camera.ProcessMouseScroll(delta);
        _needUpdate = true;
    }
}

void Scene::UpdateTexture()
{
    glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
    glBindTexture(GL_TEXTURE_2D, _texture);

    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 GL_RGB,
                 _renderSize.x,
                 _renderSize.y,
                 0,
                 GL_RGB,
                 GL_UNSIGNED_BYTE,
                 NULL);

    glBindTexture(GL_TEXTURE_2D, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
