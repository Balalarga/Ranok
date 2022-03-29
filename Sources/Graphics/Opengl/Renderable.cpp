#include "Renderable.h"

#include "Graphics/Gui/Scene.h"

#include <assert.h>
#include <iostream>


const BufferLayout BufferInfo::DefaultLayout = {
    /// Position
    LayoutItemData(GL_FLOAT, 3),
    /// Color
    LayoutItemData(GL_FLOAT, 4),
};
static const size_t DefaultLayoutSize = 7;


LayoutItemData::LayoutItemData(unsigned openglType, unsigned count):
    openglType(openglType),
    count(count)
{
    switch(openglType)
    {
    case GL_UNSIGNED_INT:
        size = sizeof(GLuint);
        break;
    case GL_SHORT:
        size = sizeof(GLchar);
        break;
    case GL_INT:
        size = sizeof(GLint);
        break;
    case GL_FLOAT:
        size = sizeof(GLfloat);
        break;
    default:
        assert(false);
    }
}

BufferInfo::BufferInfo(void *data, size_t count, unsigned drawType, const BufferLayout &layout):
    data(data),
    count(count),
    drawType(drawType),
    layout(layout),
    layoutSize(0)
{
    for (auto& i: layout)
        layoutSize += i.size * i.count;
}


Renderable::Renderable(Scene *scene, Shader *shader, const BufferInfo &vbo, const BufferInfo &ibo):
    _parent(scene),
    _handler(0),
    _shader(nullptr),
    _modelMatrix(glm::mat4(1.f))
{
    Recreate(vbo, shader, ibo);
}

Renderable::~Renderable()
{
    glDeleteVertexArrays(1, &_handler);
    delete _shader;
}

void Renderable::Render(size_t count)
{
    _shader->Bind();
    _shader->SetUniform("MVP", _modelMatrix * _parent->GetCameraViewProject());
    glBindVertexArray(_handler);
    if (_ibo.data == nullptr)
        glDrawArrays(_vbo.drawType, 0, count);
}

void Renderable::Render()
{
    _shader->Bind();
    _shader->SetUniform("MVP", _modelMatrix * _parent->GetCameraViewProject());
    glBindVertexArray(_handler);
    if (_ibo.data == nullptr)
        glDrawArrays(_vbo.drawType, 0, _vbo.count);
}

void Renderable::Recreate(const BufferInfo &vbo, Shader *shader, const BufferInfo &ibo)
{
    _vbo = vbo;

    if (_shader)
        delete _shader;

    _shader = shader;

    if (_handler != 0)
        glDeleteVertexArrays(1, &_handler);

    constexpr int buffersCount = 1;
    unsigned buffers[buffersCount];

    glGenVertexArrays(1, &_handler);
    glBindVertexArray(_handler);

    glGenBuffers(buffersCount, buffers);
    glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
    glBufferData(GL_ARRAY_BUFFER, _vbo.count * _vbo.layoutSize, _vbo.data, GL_STATIC_DRAW);

    size_t offset = 0;
    for (size_t i = 0; i < _vbo.layout.size(); ++i)
    {
        const LayoutItemData& item = _vbo.layout[i];
        glEnableVertexAttribArray(i);
        glVertexAttribPointer(i,
                              item.count,
                              item.openglType,
                              GL_FALSE,
                              _vbo.layoutSize,
                              (void*)offset);
        offset += item.count * item.size;
    }

    glBindVertexArray(0);

    glDeleteBuffers(buffersCount, buffers);
}
