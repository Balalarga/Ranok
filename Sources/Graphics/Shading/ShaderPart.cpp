#include "ShaderPart.h"

#include <vector>
#include <GL/glew.h>

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
	if (_glHandler)
		return true;
	
	if (_code.empty())
		return false;

	const char* rawSource = _code.c_str();
	_glHandler = glCreateShader(GetGLType(_type));
	
	glShaderSource(_glHandler, 1, &rawSource, 0);
	glCompileShader(_glHandler);

	if (HasError())
		return false;
	
	return true;
}

void ShaderPart::Destroy()
{
	if (_glHandler)
	{
		glDeleteShader(_glHandler);
		_glHandler = 0;
	}
}

bool ShaderPart::HasError()
{
	GLint isCompiled;
	glGetShaderiv(_glHandler, GL_COMPILE_STATUS, &isCompiled);
	if (isCompiled == GL_FALSE)
	{
		GLint maxLength;
		glGetShaderiv(_glHandler, GL_INFO_LOG_LENGTH, &maxLength);

		std::vector<GLchar> errorLog(maxLength);
		glGetShaderInfoLog(_glHandler, maxLength, &maxLength, &errorLog[0]);
		// for (auto i: errorLog)
			// std::cout<<i;
		
		// std::cout<<std::endl;

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
