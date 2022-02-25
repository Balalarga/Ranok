#pragma once

#include <GL/glew.h>
#include <string>


class Shader
{
public:
    static const std::string DefaultVertex;
    static const std::string DefaultFragment;

    Shader(const std::string& vertexCode = DefaultVertex,
           const std::string& fragmentCode = DefaultFragment,
           const std::string& geomertyCode = "");
    ~Shader();

    void Bind() const;
    void Unbind() const;


protected:
    bool HasErrors(unsigned shaderId);
    void Destroy();


private:
    const std::string _vertexCode;
    const std::string _fragmentCode;
    const std::string _geometryCode;

    unsigned _handler;
};

