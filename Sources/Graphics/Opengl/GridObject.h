#pragma once

#include "Renderable.h"

class Scene;


class GridObject: public Renderable
{
public:
    GridObject(Scene* parent);

    void Render() override;

    static constexpr size_t AxisLinesCount = 1001;


private:
    Scene* _parent;

    static constexpr glm::vec4 Color = {0.2, 0.2, 0.2, 0.5};

    static std::string defaultFragmentShader;
    static std::string defaultVertexShader;
};

