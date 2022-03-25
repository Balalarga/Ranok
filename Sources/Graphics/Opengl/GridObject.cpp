#include "GridObject.h"

#include "Graphics/Gui/Scene.h"


std::string GridObject::defaultVertexShader = R"(
#version 330

layout(location = 0) in vec3 position;
layout(location = 1) in vec4 color;

uniform mat4 MVP;

out vec4 vColor;

void main(void)
{
    gl_Position = MVP * vec4(position, 1.0);
    vColor = color;
}
)";

std::string GridObject::defaultFragmentShader = R"(
#version 330

out vec4 finalColor;

in vec4 vColor;

uniform vec4 backColor = vec4(0.5, 0.5, 0.5, 1.0);
const float FARPLANE = 1000;

void main()
{
    float distanceFromCamera = (gl_FragCoord.z / gl_FragCoord.w) / FARPLANE;
    distanceFromCamera = max(0, min(1, distanceFromCamera)); // clip to valid value range
    finalColor = mix(vColor, backColor, distanceFromCamera);
    finalColor = vColor;
}
)";


GridObject::GridObject(Scene *parent):
    Renderable(parent, new Shader(defaultVertexShader, defaultFragmentShader))
{
    static bool bIsInit = false;
    constexpr size_t count = AxisLinesCount * 4;
    constexpr float step = 0.1f;
    constexpr float length = count * step;
    constexpr float startPos = step / 2.f;
    static struct Vertex {
        float x, y, z;
        float r, g, b, a;
    } data[count];
    if (!bIsInit)
    {
        auto setter = [](Vertex& vertex, const glm::vec3& pos){
            vertex.x = pos.x;
            vertex.y = pos.y;
            vertex.z = pos.z;
            vertex.r = Color.r;
            vertex.g = Color.g;
            vertex.b = Color.b;
            vertex.a = Color.a;
        };
        for (int i = 0; i < AxisLinesCount; ++i)
        {
            float pos = startPos + i * step;
            setter(data[i], glm::vec3(pos, 0, -length));
            setter(data[i], glm::vec3(pos, 0, length));

            setter(data[i], glm::vec3(-length, 0, pos));
            setter(data[i], glm::vec3(length, 0, pos));
        }
        bIsInit = true;
    }
    SetData(BufferInfo(data, count, GL_LINES));
}

void GridObject::Render()
{
    GetShader()->Bind();
    GetShader()->SetUniform("MVP", _parent->GetCameraViewProject());
    Renderable::Render();
}
