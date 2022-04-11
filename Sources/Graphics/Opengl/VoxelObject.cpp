#include "VoxelObject.h"

#include "Graphics/Gui/Scene.h"

#include <gtc/matrix_transform.hpp>
#include <Ranok/Utility/Math.h>


std::string vertexImageShader = R"(
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

std::string fragmentImageShader = R"(
#version 330

in vec4 vColor;

void main(void)
{
    gl_FragColor = vColor;
}
)";

std::string geometryShader = R"(
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

std::string vertexModelShader = R"(
#version 330

layout(location = 0) in vec3 position;

uniform mat4 MVP;

void main(void)
{
    gl_Position = MVP * vec4(position, 1.0);
}
)";

std::string fragmentModelShader = R"(
#version 330

uniform vec4 Color;

void main(void)
{
    gl_FragColor = Color;
}
)";


float VoxelObject::PointSize = 2.f;

#include <iostream>

static bool init = false;
VoxelObject* VoxelObject::Make(Scene *parent, const Space& space, FlatArray<char> &model, glm::vec4 color)
{
    BufferLayout layout({
                            // Position
                            LayoutItemData(GL_FLOAT, 3),
                        });
    std::vector<glm::vec3> data;
    for (size_t i = 0; i < model.Size(); ++i)
    {
        if (model[i] == 0)
        {
            auto pos = space.GetPoint(i);
            data.push_back({pos[0], pos[1], pos[2]});
        }
    }

    std::cout << "Model has " << data.size() << " points of " << space.GetTotalPartition() << " points in space\n";

    BufferInfo vbo(&data[0], data.size(), GL_POINTS, layout);
    auto object = parent->AddObject<VoxelObject>(parent,
                                          new Shader(vertexModelShader, fragmentModelShader),
                                          vbo);
    object->GetShader()->AddUniform("Color");
    object->_modelColor = color;
    return object;
}

VoxelObject* VoxelObject::Make(Scene *parent, const Space& space, FlatArray<std::array<double, 5>> &image, LinearGradient& gradient, size_t activeImage)
{
    assert(activeImage < 5);

    struct Vertex
    {
        glm::fvec3 pos;
        glm::fvec4 color;
    };
    auto normalize = [](const double& value)
    {
        return UINT_MAX * (1.0 + value)/2.0;
    };

    std::vector<Vertex> data(image.Size());
    for (size_t i = 0; i < image.Size(); ++i)
    {
        auto pos = space.GetPoint(i);
        data[i].pos = {pos[0], pos[1], pos[2]};
        data[i].color = gradient.GetColor(normalize(image[i][activeImage]));
    }

    BufferInfo vbo(&data[0], data.size(), GL_POINTS);
    return parent->AddObject<VoxelObject>(parent,
                                          new Shader(vertexImageShader, fragmentImageShader),
                                          vbo);
}

VoxelObject::VoxelObject(Scene *scene, Shader *shader, const BufferInfo &vbo, const BufferInfo &ibo):
    Renderable(scene, shader, vbo, ibo),
    _voxelFilled(vbo.count)
{
    if(!init)
    {
        glEnable(GL_POINT_SIZE);
        glPointSize(PointSize);
        init = true;
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

void VoxelObject::Update(const Space& space, FlatArray<std::array<double, 5>> &image, LinearGradient &gradient, size_t activeImage)
{
    struct Vertex
    {
        glm::fvec3 pos;
        glm::fvec4 color;
    };
    auto normalize = [](const double& value)
    {
        return UINT_MAX * (1.0 + value)/2.0;
    };
    std::vector<Vertex> data(image.Size());
    for (size_t i = 0; i < image.Size(); ++i)
    {
        auto pos = space.GetPoint(i);
        data[i].pos = {pos[0], pos[1], pos[2]};
        data[i].color = gradient.GetColor(normalize(image[i][activeImage]));
    }


    glBindVertexArray(GetVao());
    glBufferSubData(GL_ARRAY_BUFFER, 0, data.size() * GetVbo().layoutSize, &data[0]);
    glBindVertexArray(0);
}

void VoxelObject::Render()
{
    GetShader()->SetUniform("Color", _modelColor);
    Renderable::Render(_voxelFilled);
}
