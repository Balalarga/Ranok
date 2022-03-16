#pragma once

#include <GL/glew.h>
#include <map>
#include <string>
#include <vector>
#include <glm.hpp>


class Shader
{
public:
    static const std::string DefaultVertex;
    static const std::string DefaultFragment;

    Shader(const std::string& vertexCode = DefaultVertex,
           const std::string& fragmentCode = DefaultFragment,
           const std::string& geomertyCode = "");
    ~Shader();

    bool AddUniform(const std::string& name);
    bool AddUniforms(const std::vector<std::string>& names);

    int GetUniformLoc(const std::string& name);

    bool UpdateVertexShader(const std::string& code);
    bool UpdateFragmentShader(const std::string& code);
    bool UpdateGeomertyShader(const std::string& code);

    bool Compile();

    void Bind() const;
    void Unbind() const;


    void SetUniform(const std::string& name, const int& value)
    {
        int loc = GetUniformLoc(name);
        if (loc >= 0)
        {
            glUniform1i(loc, value);
        }
    }

    void SetUniform(const std::string& name, const float& value)
    {
        int loc = GetUniformLoc(name);
        if (loc >= 0)
        {
            glUniform1f(loc, value);
        }
    }

    void SetUniform(const std::string& name, const glm::vec2& value)
    {
        int loc = GetUniformLoc(name);
        if (loc >= 0)
        {
            glUniform2f(loc, value.x, value.y);
        }
    }

    void SetUniform(const std::string& name, const glm::vec3& value)
    {
        int loc = GetUniformLoc(name);
        if (loc >= 0)
        {
            glUniform3f(loc, value.x, value.y, value.z);
        }
    }


protected:
    bool HasErrors(unsigned shaderId);
    void Destroy();


private:
    std::string _vertexCode;
    std::string _fragmentCode;
    std::string _geometryCode;
    std::map<std::string, int> _uniforms;

    unsigned _handler;
};

