#pragma once

#include <GL/glew.h>
#include <memory>

#include "Base/GuiItem.h"

class Renderable;


class Scene: public GuiBase
{
public:
    Scene(ImVec2 renderSize = {800, 600}, const std::string &title = "Scene");
    ~Scene();

    void Render() override;

    template<class T, class ...TArgs>
    T& AddObject(TArgs ...args)
    {
        _objects.push_back(std::make_unique<T>(args...));
        return *static_cast<T*>((_objects.back().get()));
    }

    void SetBackgroundColor(const ImVec4& color);

    inline const ImVec2& GetRenderSize() { return _renderSize; }


private:
    bool _needUpdate;
    unsigned _fbo;
    unsigned _texture;

    ImVec2 _renderSize;
    ImVec4 _backgroundColor;

    std::vector<std::unique_ptr<Renderable>> _objects;
};
