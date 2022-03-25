#ifndef RENDERABLE_H
#define RENDERABLE_H

#include <GL/glew.h>
#include <vector>

#include "Shader.h"

class Scene;


struct LayoutItemData
{
    LayoutItemData(unsigned openglType, unsigned count);

    unsigned openglType;
    unsigned size;
    unsigned count;
};

class BufferLayout: public std::vector<LayoutItemData>
{
public:
    BufferLayout(const std::vector<LayoutItemData>& data):
        std::vector<LayoutItemData>(data) {}
    BufferLayout() = default;

};

struct BufferInfo
{
    static const BufferLayout DefaultLayout;
    static const size_t DefaultLayoutSize;

    BufferInfo():
        data(nullptr) {}

    BufferInfo(void* data,
               unsigned count,
               unsigned drawType = GL_TRIANGLES,
               const BufferLayout& layout = DefaultLayout);

    void* data;
    unsigned count;
    unsigned drawType;
    unsigned layoutSize;
    BufferLayout layout;
};


class Renderable
{
public:
    Renderable(Scene* parent,
               Shader* shader,
               const BufferInfo& vbo = BufferInfo(nullptr, 0, {}),
               const BufferInfo& ibo = BufferInfo(nullptr, 0, {}));
    ~Renderable();

    bool SetData(const BufferInfo& vbo, const BufferInfo& ibo = BufferInfo(nullptr, 0, {}));

    virtual void Render(unsigned count);
    virtual void Render();


protected:
    inline Shader* GetShader() { return _shader; }
    inline const unsigned& GetVao() { return _handler; }
    inline const BufferInfo& GetVbo() { return _vbo; }
    inline Scene* Parent() const { return _parent; }


private:
    BufferInfo _vbo;
    BufferInfo _ibo;
    Shader* _shader;

    unsigned _handler;
    Scene* _parent;
};

#endif // RENDERABLE_H
