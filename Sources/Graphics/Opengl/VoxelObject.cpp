#include "VoxelObject.h"

#include "Graphics/Gui/Scene.h"

#include <gtc/matrix_transform.hpp>


std::string VoxelObject::defaultVertexShader = R"(
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

std::string VoxelObject::defaultGeometryShader = R"(
#version 330

layout(points) in;
layout(triangle_strip, max_vertices = 24) out;

uniform vec3 voxelSize = vec3(0.1, 0.1, 0.1);
uniform mat4 MVP;

in vec4 vColor[];
out vec4 gColor;

void AddQuad(vec4 center, vec4 dy, vec4 dx)
{
    gl_Position = center + (dx - dy);
    EmitVertex();
    gl_Position = center + (-dx - dy);
    EmitVertex();
    gl_Position = center + (dx + dy);
    EmitVertex();
    gl_Position = center + (-dx + dy);
    EmitVertex();
    EndPrimitive();
}

void main()
{
    gColor = vColor[0];
    vec4 center = gl_in[0].gl_Position;

    vec4 dx = MVP[0]/2.0f * voxelSize.x;
    vec4 dy = MVP[1]/2.0f * voxelSize.y;
    vec4 dz = MVP[2]/2.0f * voxelSize.z;

    AddQuad(center + dx, dy, dz);
    AddQuad(center - dx, dz, dy);
    AddQuad(center + dy, dz, dx);
    AddQuad(center - dy, dx, dz);
    AddQuad(center + dz, dx, dy);
    AddQuad(center - dz, dy, dx);
}
)";

std::string VoxelObject::defaultFragmentShader = R"(
#version 330

in vec4 gColor;

void main(void)
{
    gl_FragColor = gColor;
}
)";


float VoxelObject::PointSize = 10.f;


VoxelObject::VoxelObject(Scene* parent, void* data, size_t size):
    Renderable(parent, new Shader(), BufferInfo(data, size, GL_POINTS)),
    _voxelsCount(size),
    _voxelFilled(0)
{
    static bool init = false;
    if(!init)
    {
        glEnable(GL_POINT_SIZE);
        glPointSize(PointSize);
    }
}

void VoxelObject::SetSubData(void *begin, size_t count)
{
    glBindVertexArray(GetVao());

    unsigned offset = _voxelFilled * GetVbo().layoutSize;
    glBufferSubData(GL_ARRAY_BUFFER, offset, count * GetVbo().layoutSize, begin);

    _voxelFilled += count;
    glBindVertexArray(0);
}

void VoxelObject::Render()
{
    Renderable::Render(_voxelFilled);
}