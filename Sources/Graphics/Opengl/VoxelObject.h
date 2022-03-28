#ifndef VOXELOBJECT_H
#define VOXELOBJECT_H

#include "Renderable.h"

class Scene;


class VoxelObject: public Renderable
{
public:
    VoxelObject(Scene* parent, void* data, size_t size);

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
