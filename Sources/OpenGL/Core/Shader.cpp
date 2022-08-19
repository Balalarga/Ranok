#include "Shader.h"

#include <vector>
#include <iostream>

#include "OpenGL/ErrorHandle.h"

#include "IRenderable.h"


Shader::Shader(ShaderPart* vShader, ShaderPart* fShader, ShaderPart* gShader):
    _parts({vShader, fShader, gShader})
{
}

Shader::~Shader()
{
    Destroy();
}

void Shader::AttachTo(IRenderable* object)
{
    _renderableObjects.emplace(object);
}

void Shader::DetachFrom(IRenderable* object)
{
    _renderableObjects.erase(object);
}

void Shader::BatchRender()
{
    for (auto& obj: _renderableObjects)
        obj->Render();
}

bool Shader::Compile(bool bCompileParts)
{
    _handler = glCreateProgram();

    std::vector parts{_parts.vShader, _parts.fShader};
    if (_parts.gShader)
        parts.push_back(_parts.gShader);
    
    std::vector<unsigned> attachedShaders;
    for (auto& part : parts)
    {
        if (!part->IsInited() && (!bCompileParts || !part->Compile()))
            continue;
        
        attachedShaders.push_back(part->GetHandler());
        GLCall(glAttachShader(_handler, part->GetHandler()))
    }
    GLCall(glLinkProgram(_handler))
    
    return attachedShaders.size() == parts.size();
}

void Shader::Bind() const
{
    GLCall(glUseProgram(_handler))
}

void Shader::Unbind() const
{
    GLCall(glUseProgram(0))
}

bool Shader::HasErrors(unsigned shaderId)
{
    GLint isCompiled = 0;
    glGetShaderiv(shaderId, GL_COMPILE_STATUS, &isCompiled);
    if (isCompiled == GL_FALSE)
    {
        GLint maxLength = 0;
        glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &maxLength);

        std::vector<GLchar> errorLog(maxLength);
        glGetShaderInfoLog(shaderId, maxLength, &maxLength, &errorLog[0]);
        for (auto i: errorLog)
            std::cout<<i;
        std::cout<<std::endl;

        return true;
    }
    return false;
}

void Shader::Destroy()
{
    if (_handler)
    {
        Unbind();
        std::vector parts{_parts.vShader, _parts.fShader};
        if (_parts.gShader)
            parts.push_back(_parts.gShader);

        for (ShaderPart*& p : parts)
        {
            GLCall(glDetachShader(_handler, p->GetHandler()))
        }
        
        GLCall(glDeleteProgram(_handler))
        _handler = 0;
    }
}

bool Shader::AddUniform(const std::string &name)
{
    int loc = glGetUniformLocation(_handler, name.c_str());
    if (loc < 0)
    {
        Unbind();
        return false;
    }

    _uniforms[name] = loc;

    return true;
}

bool Shader::AddUniforms(const std::vector<std::string> &names)
{
    bool allOk = true;
    for (auto& name : names)
    {
        int loc = glGetUniformLocation(_handler, name.c_str());

        if (loc < 0)
        {
            std::cout << "Couldn't find "<< name << " uniform\n";
            allOk = false;
        }
        else
        {
            _uniforms[name] = loc;
        }
    }

    return allOk;
}

void Shader::SetUniform(const std::string &name, const int &value)
{
    int loc = GetUniformLoc(name);
    if (loc >= 0)
    {
        GLCall(glUniform1i(loc, value))
    }
}

void Shader::SetUniform(const std::string &name, const float &value)
{
    int loc = GetUniformLoc(name);
    if (loc >= 0)
    {
        GLCall(glUniform1f(loc, value))
    }
}

void Shader::SetUniform(const std::string &name, const glm::vec2 &value)
{
    int loc = GetUniformLoc(name);
    if (loc >= 0)
    {
        GLCall(glUniform2f(loc, value.x, value.y))
    }
}

void Shader::SetUniform(const std::string &name, const glm::vec3 &value)
{
    int loc = GetUniformLoc(name);
    if (loc >= 0)
    {
        GLCall(glUniform3f(loc, value.x, value.y, value.z))
    }
}

void Shader::SetUniform(const std::string &name, const glm::vec4 &value)
{
    int loc = GetUniformLoc(name);
    if (loc >= 0)
    {
        GLCall(glUniform4f(loc, value.x, value.y, value.z, value.w))
    }
}

void Shader::SetUniform(const std::string &name, const glm::mat4 &value)
{
    int loc = GetUniformLoc(name);
    if (loc >= 0)
    {
        GLCall(glUniformMatrix4fv(loc, 1, GL_FALSE, &value[0][0]))
    }
}

void Shader::SetUniform(const std::string &name, const glm::mat3 &value)
{
    int loc = GetUniformLoc(name);
    if (loc >= 0)
    {
        GLCall(glUniformMatrix3fv(loc, 1, GL_FALSE, &value[0][0]))
    }
}

int Shader::GetUniformLoc(const std::string& name)
{
    auto it = _uniforms.find(name);
    return it != _uniforms.end() ? it->second : -1;
}
