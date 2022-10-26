#include "BaseMaterial.h"

BaseMaterial::BaseMaterial(std::shared_ptr<Shader> shader):
	IMaterial(std::move(shader)),
	_color(0)
{
}

void BaseMaterial::SetupUniforms()
{
	SetUniform("uColor", _color);
}
