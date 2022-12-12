#version 330

in vec2 vertUv;
in vec4 vertColor;

out vec4 fragColor;

void main()
{
    fragColor = vertColor;
}