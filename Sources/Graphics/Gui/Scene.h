#pragma once

#include <GL/glew.h>

#include "Base/GuiItem.h"

class Renderable;


class Scene: public GuiItem
{
public:
    Scene(ImVec2 size = {800, 600}, const std::string &title = "Scene");
    ~Scene();

    void Render();

    void AddObject(Renderable* object);

    void SetBackgroundColor(const ImVec4& color);


private:
    unsigned _fbo;
    unsigned _texture;

    std::string _title;
    ImVec2 _size;
    ImVec4 _backgroundColor;

    std::vector<Renderable*> _objects;
};
