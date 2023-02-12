#include "VoxelModel.h"

#include <utility>

namespace Ranok
{

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

static std::shared_ptr<Shader> GetDefaultShader()
{
    static std::shared_ptr<ShaderPart> vShader = std::make_shared<ShaderPart>(ShaderType::Vertex, vertexImageShader);
    static std::shared_ptr<ShaderPart> fShader = std::make_shared<ShaderPart>(ShaderType::Fragment, fragmentImageShader);
    // static std::shared_ptr<ShaderPart> gShader = std::make_shared<ShaderPart>(ShaderType::Geometry, geometryShader);
    static std::shared_ptr<Shader> _defaultShader = std::make_shared<Shader>(vShader, fShader);
    return _defaultShader;
}

static std::shared_ptr<VoxelMaterial> GetDefaultMaterial()
{
    static std::shared_ptr material(std::make_shared<VoxelMaterial>());
    return material;
}

VoxelMaterial::VoxelMaterial():
    IMaterial(GetDefaultShader())
{
}

void VoxelMaterial::SetupUniforms()
{

}

VoxelModel* VoxelModel::Make(const Space3D& space,
                            FlatArray<MImage3D>& image,
                            LinearGradient& gradient,
                            size_t activeImage)
{
    struct Vertex
    {
        glm::fvec3 pos;
        glm::fvec4 color;
    };
    std::vector<Vertex> data(image.Size());
    for (size_t i = 0; i < image.Size(); ++i)
    {
        std::vector<float> pos = space.GetPoint(i);
        data[i].pos = {pos[0], pos[1], pos[2]};
        data[i].color = gradient.GetColor(UINT_MAX * (1.0 + image[i].c[activeImage])/2.0);
    }

    // BufferInfo vbo(&data[0], data.size(), GL_POINTS);
    // return parent->AddObject<VoxelObject>(parent,
                                          // new Shader(vertexImageShader, fragmentImageShader),
                                          // vbo);
    Buffer buffer(data, BufferLayout().Float(3).Float(4), GL_POINTS);
    return new VoxelModel(buffer);
}

VoxelModel::VoxelModel(Buffer vbo):
    Object(std::move(vbo), nullptr)
{
    SetMaterial(&_material);
}

void VoxelModel::Update(const Space3D& space,
                        FlatArray<std::array<double, 5>>& image,
                        LinearGradient& gradient,
                        size_t activeImage)
{

}

void VoxelModel::Render() const
{
	Object::Render();
}
}
