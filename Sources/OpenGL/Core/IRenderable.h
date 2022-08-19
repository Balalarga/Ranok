#ifndef RENDERABLE_H
#define RENDERABLE_H

#include <vector>
#include <memory>
#include <GL/glew.h>

#include "Shader.h"
#include "Buffer.h"


class IRenderable
{
public:
    IRenderable(const Buffer& vbo = {});
    virtual ~IRenderable();

    virtual void Render();

    bool Setup(const Buffer& vbo);
    
    void SetShader(Shader* shader);
    Shader* GetShader() { return _shaderPtr; }
    
    void Bind();
    void Release();

    void SetVisible(bool bVisible) { _bVisible = bVisible; }
    

private:
    bool _bVisible = true;
    
    Buffer _vbo;
    GLuint _handler;
    Shader* _shaderPtr{};

    glm::mat4 _modelMatrix;
};

#endif // RENDERABLE_H
