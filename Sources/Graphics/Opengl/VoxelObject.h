#ifndef VOXELOBJECT_H
#define VOXELOBJECT_H

#include "Renderable.h"

#include <Ranok/Core/Space.h>
#include <Ranok/Core/Utils/FlatArray.h>

class Scene;


class VoxelObject: public Renderable
{
public:
    VoxelObject(Scene* parent, Space space, FlatArray<char>& model, glm::vec4 color = glm::vec4(0.3, 0.3, 0.3, 0.5));
    VoxelObject(Scene* parent, Space space, FlatArray<double>& image);

    void SetSubData(void* begin, size_t count);

    virtual void Render();


private:
    static std::string defaultFragmentShader;
    static std::string defaultVertexShader;
    static std::string defaultGeometryShader;
    static float PointSize;

    size_t _voxelsCount;
    size_t _voxelFilled;
};

#endif // VOXELOBJECT_H
