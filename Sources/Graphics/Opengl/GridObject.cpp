#include "GridObject.h"


size_t GridObject::LinesCount = 1000;


glm::vec4 GridObject::Color = {0.5, 0.5, 0.5, 1.0};


std::string GridObject::DefaultVertexShader = R"(
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


std::string GridObject::DefaultFragmentShader = R"(
#version 330

out vec4 finalColor;

in vec4 vColor;

uniform vec4 backColor = vec4(0.8, 0.8, 0.8, 1.0);
const float FARPLANE = 1000;

void main()
{
    float distanceFromCamera = (gl_FragCoord.z / gl_FragCoord.w) / FARPLANE;
    distanceFromCamera = max(0, min(1, distanceFromCamera)); // clip to valid value range
    finalColor = mix(vColor, backColor, distanceFromCamera);
//    finalColor = vColor;
}
)";
