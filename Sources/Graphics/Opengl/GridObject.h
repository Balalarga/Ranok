#pragma once

#include "Renderable.h"

class Scene;


class GridObject: public Renderable
{
public:
    GridObject(Scene* parent);

    void Render() override;


    static constexpr size_t LinesCount = 1000;

private:
    Scene* _parent;


    static std::array<float, BufferInfo::DefaultLayoutSize> _data;

    static std::string defaultFragmentShader;
    static std::string defaultVertexShader;
};

