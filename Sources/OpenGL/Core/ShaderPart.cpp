#include "ShaderPart.h"

#include <GL/glew.h>
#include <vector>
#include <iostream>

#include "OpenGL/ErrorHandle.h"


ShaderPart::ShaderPart(Type type, const std::string& code):
	_type(type),
	_code(code)
{
}

ShaderPart::~ShaderPart()
{
	Destroy();
}

void ShaderPart::Setup(Type type, const std::string& code)
{
	_type = type;
	_code = code;
}

bool ShaderPart::Compile()
{
	if (_code.empty())
		return false;

	const char* rawSource = _code.c_str();
	_handler = glCreateShader(GetGLType(_type));
	
	GLCall(glShaderSource(_handler, 1, &rawSource, 0))
	GLCall(glCompileShader(_handler))

	if (HasError())
	{
		std::cout << "Shader compilation error\n" << _code << std::endl;
		return false;
	}
	
	return true;
}

void ShaderPart::Destroy()
{
	if (_handler)
	{
		GLCall(glDeleteShader(_handler))
		_handler = 0;
	}
}

bool ShaderPart::HasError()
{
	GLint isCompiled;
	glGetShaderiv(_handler, GL_COMPILE_STATUS, &isCompiled);
	if (isCompiled == GL_FALSE)
	{
		GLint maxLength;
		glGetShaderiv(_handler, GL_INFO_LOG_LENGTH, &maxLength);

		std::vector<GLchar> errorLog(maxLength);
		glGetShaderInfoLog(_handler, maxLength, &maxLength, &errorLog[0]);
		for (auto i: errorLog)
			std::cout<<i;
		
		std::cout<<std::endl;

		return true;
	}
	return false;
}

unsigned ShaderPart::GetGLType(Type type)
{
	switch (type)
	{
		case Type::Vertex:
			return GL_VERTEX_SHADER;
		case Type::Fragment:
			return GL_FRAGMENT_SHADER;
		case Type::Geometry:
			return GL_GEOMETRY_SHADER;
	}
	return 0;
}
