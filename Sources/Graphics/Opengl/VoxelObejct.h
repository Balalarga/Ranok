#ifndef VOXELOBEJCT_H
#define VOXELOBEJCT_H

#include "Renderable.h"

class Scene;


class VoxelObejct: public Renderable
{
public:
    VoxelObejct(unsigned size, void* data, Scene* parent);

    void Render() override;
    void Render(unsigned count) override;

    void SetSubData(void* begin, unsigned count);


private:
    static std::string defaultFragmentShader;
    static std::string defaultVertexShader;
    static std::string defaultGeometryShader;

    unsigned _voxelsCount;
    unsigned _voxelFilled;

    glm::mat4 _model;
    glm::mat4 _view;
    glm::mat4 _projection;
};

#endif // VOXELOBEJCT_H
