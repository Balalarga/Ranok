#include "Scene.h"
#include "Graphics/Opengl/Base/Renderable.h"

#include <functional>


Scene::Scene(ImVec2 renderSize, const std::string& title):
    GuiBase(title),
    _renderSize(renderSize),
    _backgroundColor({0.5, 0.5, 0.5, 1.0}),
    _needUpdate(true)
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
}

Scene::~Scene()
{
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


    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 0.0f));
    ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 0);
    ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
    ImGui::BeginChild(Name().c_str(), Size());

    ImGui::Image((void*)(intptr_t)_texture, ImGui::GetWindowSize(), ImVec2(0, 1), ImVec2(1, 0));

    ImGui::EndChild();
    ImGui::PopStyleVar(4);
}

void Scene::SetBackgroundColor(const ImVec4 &color)
{
    _backgroundColor = color;
}
