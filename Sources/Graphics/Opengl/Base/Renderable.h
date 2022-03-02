#ifndef RENDERABLE_H
#define RENDERABLE_H

#include <GL/glew.h>
#include <vector>

#include "Shader.h"


struct LayoutItemData
{
    LayoutItemData(unsigned openglType, unsigned count);

    unsigned openglType;
    unsigned size;
    unsigned count;
};

using BufferLayout = std::vector<LayoutItemData>;

struct BufferInfo
{
    static const BufferLayout DefaultLayout;

    BufferInfo(void* data, unsigned count, const BufferLayout& layout = DefaultLayout);

    void* data;
    unsigned count;
    BufferLayout layout;
};


class Renderable
{
public:
    Renderable(const BufferInfo& vbo,
               Shader* shader = nullptr,
               const BufferInfo& ibo = BufferInfo(nullptr, 0, {}));
    ~Renderable();

    void Render();


private:
    BufferInfo _vbo;
    BufferInfo _ibo;
    Shader* _shader;

    unsigned _handler;
};

#endif // RENDERABLE_H
