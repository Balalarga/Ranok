#include "Shader.h"

#include <vector>
#include <iostream>


const std::string Shader::DefaultVertex = R"(
#version 330

layout(location = 0) in vec3 iPosition;
layout(location = 1) in vec4 iColor;

uniform mat4 MVP = mat4(1.0);


out vec4 vColor;


void main(void)
{
    gl_Position = MVP * vec4(iPosition, 1.0);
    vColor = iColor;
}
)";

const std::string Shader::DefaultFragment = R"(
#version 330

in vec4 vColor;


out vec4 fFragColor;


void main(void)
{
    fFragColor = vColor;
}
)";


Shader::Shader(const std::string &vertexCode,
               const std::string &fragmentCode,
               const std::string &geomertyCode):
    _vertexCode(vertexCode),
    _fragmentCode(fragmentCode),
    _geometryCode(geomertyCode),
    _handler(0)
{
    std::vector<std::pair<unsigned, const std::string&>> shaders
    {
        {GL_VERTEX_SHADER, _vertexCode},
        {GL_FRAGMENT_SHADER, _fragmentCode},
        {GL_GEOMETRY_SHADER, _geometryCode},
    };

    _handler = glCreateProgram();

    std::vector<unsigned> attachedShaders;
    for(auto& [type, code]: shaders)
    {
        if (code.empty())
            continue;

        const unsigned id = glCreateShader(type);
        const char* rawSource = code.c_str();

        glShaderSource(id, 1, &rawSource, 0);
        glCompileShader(id);

        if (HasErrors(id))
        {
            std::cout << "Shader compilation error\n" << code << std::endl;
            glDeleteShader(id);
            for(auto& i: attachedShaders)
            {
                glDetachShader(_handler, i);
                glDeleteShader(i);
            }

            Destroy();
            return;
        }

        attachedShaders.push_back(_handler);
        glAttachShader(_handler, id);
    }

    glLinkProgram(_handler);

    for(auto& i: attachedShaders)
    {
        glDetachShader(_handler, i);
        glDeleteShader(i);
    }
}

bool Shader::HasErrors(unsigned shaderId)
{
    GLint isCompiled = 0;
    glGetShaderiv(shaderId, GL_COMPILE_STATUS, &isCompiled);
    if(isCompiled == GL_FALSE)
    {
        GLint maxLength = 0;
        glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &maxLength);

        std::vector<GLchar> errorLog(maxLength);
        glGetShaderInfoLog(shaderId, maxLength, &maxLength, &errorLog[0]);
        for(auto i: errorLog)
        {
            std::cout<<i;
        }
        std::cout<<std::endl;

        return true;
    }
    return false;
}

void Shader::Destroy()
{
    glDeleteProgram(_handler);
}

Shader::~Shader()
{
    Destroy();
}

void Shader::Bind() const
{
    glUseProgram(_handler);
}

void Shader::Unbind() const
{
    glUseProgram(0);
}
