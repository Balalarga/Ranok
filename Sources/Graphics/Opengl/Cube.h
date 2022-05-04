#ifndef RANOK_CUBE_H
#define RANOK_CUBE_H

#include "Renderable.h"


class Cube: public Renderable
{
public:
    Cube(Scene* scene, glm::vec3 pos, float size);

};


#endif //RANOK_CUBE_H
