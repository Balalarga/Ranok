#ifndef VOXELOBJECT_H
#define VOXELOBJECT_H

#include "Renderable.h"

#include <Ranok/Core/GradientModel.h>
#include <Ranok/Core/Space.h>
#include <Ranok/Core/Utils/FlatArray.h>

class Scene;


class VoxelObject: public Renderable
{
public:
    static VoxelObject* Make(Scene* parent,
                             const Space& space,
                             FlatArray<MImage3D> &image,
                             LinearGradient& gradient,
                             size_t activeImage);

    VoxelObject(Scene* scene,
                Shader* shader,
                const BufferInfo& vbo,
                const BufferInfo& ibo = BufferInfo(nullptr, 0, {}));

    void SetSubData(void* begin, size_t count);
    void Update(const Space& space, FlatArray<std::array<double, 5>> &image, LinearGradient& gradient, size_t activeImage);

    virtual void Render();


private:
    size_t _voxelsCount;
    size_t _voxelFilled;
    glm::fvec4 _modelColor;
};

#endif // VOXELOBJECT_H
