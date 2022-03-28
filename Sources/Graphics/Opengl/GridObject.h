#pragma once

#include <string>
#include <glm.hpp>


class GridObject
{
public:
    GridObject() = delete;

    static size_t LinesCount;
    static glm::vec4 Color;
    static std::string DefaultFragmentShader;
    static std::string DefaultVertexShader;
};
