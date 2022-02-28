#include "Scene.h"
#include <functional>


Scene::Scene(ImVec2 size, const std::string& title):
    GuiItem(std::bind(&Scene::Render, this)),
    _title(title),
    _size(size),
    _backgroundColor({0.5, 0.5, 0.5, 1.0})
{
    glGenFramebuffers(1, &_fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, _fbo);

    glGenTextures(1, &_texture);
    glBindTexture(GL_TEXTURE_2D, _texture);

    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 GL_RGB,
                 _size.x,
                 _size.y,
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
    for(auto& i: _objects)
        delete i;

    glDeleteFramebuffers(1, &_fbo);
}

void Scene::Render()
{
    glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
    glViewport(0, 0, _size.x, _size.y);
    glClearColor(_backgroundColor.x,
                 _backgroundColor.y,
                 _backgroundColor.z,
                 _backgroundColor.w);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    for(auto& i: _objects)
        i->Render();

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));

    ImGui::Begin("Scene", nullptr, ImGuiWindowFlags_NoTitleBar);
    ImGui::Image((void*)(intptr_t)_texture, ImGui::GetWindowSize(), ImVec2(0, 1), ImVec2(1, 0));
    ImGui::End();

    ImGui::ShowDemoWindow();
    ImGui::PopStyleVar();
}

void Scene::AddObject(Renderable *object)
{
    if (object)
        _objects.push_back(object);
}

void Scene::SetBackgroundColor(const ImVec4 &color)
{
    _backgroundColor = color;
}
