#include "Renderable.h"

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

BufferInfo::BufferInfo(void *data, unsigned count, unsigned drawType, const BufferLayout &layout):
    data(data),
    count(count),
    drawType(drawType),
    layout(layout),
    layoutSize(0)
{
    for (auto& i: layout)
        layoutSize += i.size * i.count;
}


Renderable::Renderable(const BufferInfo &vbo, Shader *shader, const BufferInfo &ibo):
    _handler(0),
    _vbo(vbo),
    _ibo(ibo),
    _shader(shader)
{
    if (!_shader)
        _shader = new Shader();

    constexpr int buffersCount = 2;
    unsigned buffers[buffersCount];

    glGenVertexArrays(1, &_handler);
    glBindVertexArray(_handler);

    glGenBuffers(buffersCount, buffers);
    glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
    glBufferData(GL_ARRAY_BUFFER, _vbo.count * _vbo.layoutSize, _vbo.data, GL_STATIC_DRAW);

    long long offset = 0;
    for (int i = 0; i < _vbo.layout.size(); ++i)
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
}

Renderable::~Renderable()
{
    glDeleteVertexArrays(1, &_handler);
    delete _shader;
}

void Renderable::Render(unsigned count)
{
    _shader->Bind();
    glBindVertexArray(_handler);
    if (_ibo.data == nullptr)
        glDrawArrays(_vbo.drawType, 0, count);
}

void Renderable::Render()
{
    _shader->Bind();
    glBindVertexArray(_handler);
    if (_ibo.data == nullptr)
        glDrawArrays(_vbo.drawType, 0, _vbo.count);
}
