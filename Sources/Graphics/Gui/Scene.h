#pragma once

#include <GL/glew.h>

#include "Base/GuiItem.h"

class Renderable;


class Scene: public GuiChildWindow
{
public:
    Scene(ImVec2 renderSize = {800, 600}, const std::string &title = "Scene");
    ~Scene();

    void PreRender() override;
    void OnRender() override;
    void PostRender() override;

    void AddObject(Renderable* object);

    void SetBackgroundColor(const ImVec4& color);

    inline const ImVec2& GetRenderSize() { return _renderSize; }


private:
    unsigned _fbo;
    unsigned _texture;

    ImVec2 _renderSize;
    ImVec4 _backgroundColor;

    std::vector<Renderable*> _objects;
};
