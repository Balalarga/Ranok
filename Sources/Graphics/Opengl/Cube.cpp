#include "Cube.h"


Cube::Cube(Scene *scene, glm::vec3 pos, float size):
    Renderable(scene, new Shader(Shader::DefaultVertex, Shader::DefaultFragment), BufferInfo())
{
    glm::vec4 color = glm::vec4(1.0, 0.2, 0.2, 1.0);
    struct Vertex
    {
        float x, y, z;
        float r, g, b, a;
    };
    std::vector<Vertex> data{
            { pos.x-size,  pos.y-size  , pos.z-size, color.x, color.y, color.z, color.w },
             { pos.x-size,  pos.y-size  , pos.z +size, color.x, color.y, color.z, color.w },
              { pos.x-size,  pos.y +size, pos.z +size, color.x, color.y, color.z, color.w },
               { pos.x+size, pos.y +size, pos.z-size, color.x, color.y, color.z, color.w },
                { pos.x-size,  pos.y-size  , pos.z-size, color.x, color.y, color.z, color.w },
                 { pos.x-size,  pos.y +size, pos.z-size, color.x, color.y, color.z, color.w },
                  { pos.x+size, pos.y-size  , pos.z +size, color.x, color.y, color.z, color.w },
                   { pos.x-size,  pos.y-size  , pos.z-size, color.x, color.y, color.z, color.w },
                    { pos.x+size, pos.y-size  , pos.z-size, color.x, color.y, color.z, color.w },
                     { pos.x+size, pos.y +size, pos.z-size, color.x, color.y, color.z, color.w },
                      { pos.x+size, pos.y-size  , pos.z-size, color.x, color.y, color.z, color.w },
                       { pos.x-size , pos.y-size  , pos.z-size, color.x, color.y, color.z, color.w },
                        { pos.x-size , pos.y-size  , pos.z-size, color.x, color.y, color.z, color.w },
                         { pos.x-size , pos.y +size, pos.z +size, color.x, color.y, color.z, color.w },
                          { pos.x-size , pos.y +size, pos.z-size, color.x, color.y, color.z, color.w },
                           { pos.x+size, pos.y-size  , pos.z +size, color.x, color.y, color.z, color.w },
                            { pos.x-size , pos.y-size  , pos.z +size, color.x, color.y, color.z, color.w },
                             { pos.x-size , pos.y-size  , pos.z-size, color.x, color.y, color.z, color.w },
                              { pos.x-size , pos.y+size , pos.z +size, color.x, color.y, color.z, color.w },
                               { pos.x-size , pos.y-size  , pos.z +size, color.x, color.y, color.z, color.w },
                                { pos.x+size, pos.y-size  , pos.z +size, color.x, color.y, color.z, color.w },
                                 { pos.x+size, pos.y +size, pos.z +size, color.x, color.y, color.z, color.w },
                                  { pos.x+size, pos.y-size  , pos.z-size, color.x, color.y, color.z, color.w },
                                   { pos.x+size, pos.y +size, pos.z-size, color.x, color.y, color.z, color.w },
                                    { pos.x+size, pos.y-size  , pos.z-size, color.x, color.y, color.z, color.w },
                                     { pos.x+size, pos.y +size, pos.z +size, color.x, color.y, color.z, color.w },
                                      { pos.x+size, pos.y-size  , pos.z +size, color.x, color.y, color.z, color.w },
                                       { pos.x+size, pos.y +size, pos.z +size, color.x, color.y, color.z, color.w },
                                        { pos.x+size, pos.y +size, pos.z-size, color.x, color.y, color.z, color.w },
                                         { pos.x-size , pos.y +size, pos.z-size, color.x, color.y, color.z, color.w },
                                          { pos.x+size, pos.y +size, pos.z +size, color.x, color.y, color.z, color.w },
                                           { pos.x-size , pos.y +size, pos.z-size, color.x, color.y, color.z, color.w },
                                            { pos.x-size , pos.y +size, pos.z +size, color.x, color.y, color.z, color.w },
                                             { pos.x+size, pos.y +size, pos.z +size, color.x, color.y, color.z, color.w },
                                              { pos.x-size , pos.y +size, pos.z +size, color.x, color.y, color.z, color.w },
                                               { pos.x+size, pos.y-size  , pos.z +size, color.x, color.y, color.z, color.w },
    };

    BufferInfo vbo(&data[0], data.size());
    Recreate(vbo);
}
