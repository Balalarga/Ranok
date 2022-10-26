#include "BufferLayout.h"
#include <GL/glew.h>

BufferLayout& BufferLayout::Float(unsigned count)
{
	Variables.push_back({GL_FLOAT, count, sizeof(GLfloat)}); Size += count * sizeof(GLfloat);
	return *this;
}

BufferLayout& BufferLayout::Int(unsigned count)
{
	Variables.push_back({GL_INT, count, sizeof(GLint)}); Size += count * sizeof(GLint);
	return *this;
}

BufferLayout& BufferLayout::Unsigned(unsigned count)
{
	Variables.push_back({GL_UNSIGNED_INT, count, sizeof(GLuint)}); Size += count * sizeof(GLuint);
	return *this;
}
