#include "Object.h"

#include <cassert>
#include <GL/glew.h>

#include "Graphics/Materials/IMaterial.h"

Object::Object(Buffer vbo, IMaterial* material):
	_vbo(std::move(vbo)),
	_material(material)
{
	assert(_vbo.Data.Ptr);
    
	glGenVertexArrays(1, &_glHandler);
	glBindVertexArray(_glHandler);

	const unsigned vboId = _vbo.Create();
	glBindVertexArray(0);
    
	glDeleteBuffers(1, &vboId);
}

Object::~Object()
{
	glDeleteVertexArrays(1, &_glHandler);
}

void Object::SetMaterial(IMaterial* material)
{
	_material = material;
}

void Object::Render() const
{
	if (_material)
	{
		_material->Bind();
		_material->SetupUniforms();
	}
	
	glBindVertexArray(_glHandler);
	glDrawArrays(_vbo.DrawType, 0, _vbo.Data.Count);
}
