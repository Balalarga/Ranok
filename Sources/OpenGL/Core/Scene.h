#pragma once

#include <memory>
#include <vector>

#include "IRenderable.h"


class Scene
{
public:
    Scene() = default;
    virtual ~Scene() = default;

    template<class T>
    T& AddObject(T*&& Obj)
    {
        _renderObjects.push_back(std::unique_ptr<T>(Obj));
        return *_renderObjects.back().get();
    }

    const std::vector<std::unique_ptr<IRenderable>>& GetObjects() const { return _renderObjects; }
    std::set<Shader*> GetShaders() const;


private:
    std::vector<std::unique_ptr<IRenderable>> _renderObjects;
};
