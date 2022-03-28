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

using BufferLayout = std::vector<LayoutItemData>;

struct BufferInfo
{
    static const BufferLayout DefaultLayout;
    static const size_t DefaultLayoutSize;

    BufferInfo(void* data = nullptr,
               size_t count = 0,
               unsigned drawType = GL_TRIANGLES,
               const BufferLayout& layout = DefaultLayout);

    void* data;
    size_t count;
    unsigned drawType;
    size_t layoutSize;
    BufferLayout layout;
};


class Renderable
{
public:
    Renderable(Scene* scene,
               Shader* shader,
               const BufferInfo& vbo,
               const BufferInfo& ibo = BufferInfo(nullptr, 0, {}));
    ~Renderable();

    virtual void Render(size_t count);
    virtual void Render();

    void Recreate(const BufferInfo& vbo,
                  Shader* shader = nullptr,
                  const BufferInfo& ibo = BufferInfo(nullptr, 0, {}));


protected:
    inline Scene* Parent() { return _parent; }
    inline Shader* GetShader() { return _shader; }
    inline const unsigned& GetVao() { return _handler; }
    inline const BufferInfo& GetVbo() { return _vbo; }


private:
    BufferInfo _vbo;
    BufferInfo _ibo;
    Shader* _shader;
    Scene* _parent;

    unsigned _handler;
    glm::mat4 _modelMatrix;
};

#endif // RENDERABLE_H
