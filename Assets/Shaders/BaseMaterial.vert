#version 330

layout(location = 0)in vec3 iVert;
layout(location = 1)in vec4 iVertColor;

uniform mat4 uMvp = mat4(1);

out vec4 vertColor;
out vec2 vertUv;

void main()
{
    gl_Position = uMvp * vec4(iVert, 1.0);
    vertColor = iVertColor;
}