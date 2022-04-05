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

    bool UpdateVertexShader(const std::string &code);
    bool UpdateFragmentShader(const std::string &code);
    bool UpdateGeomertyShader(const std::string &code);

    bool AddUniform(const std::string& name);
    bool AddUniforms(const std::vector<std::string>& names);

    int GetUniformLoc(const std::string& name);

    bool Compile();

    void Bind() const;
    void Unbind() const;


    void SetUniform(const std::string& name, const int& value);
    void SetUniform(const std::string& name, const float& value);
    void SetUniform(const std::string& name, const glm::vec2& value);
    void SetUniform(const std::string& name, const glm::vec3& value);
    void SetUniform(const std::string& name, const glm::vec4& value);
    void SetUniform(const std::string& name, const glm::mat4& value);
    void SetUniform(const std::string& name, const glm::mat3& value);


protected:
    void Destroy();
    bool HasErrors(unsigned shaderId);


private:
    std::string _vertexCode;
    std::string _fragmentCode;
    std::string _geometryCode;
    std::map<std::string, int> _uniforms;

    GLuint _handler;
};

