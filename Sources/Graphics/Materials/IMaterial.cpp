#include "IMaterial.h"

#include <utility>

IMaterial::IMaterial(std::shared_ptr<Shader> shader):
	_shader(std::move(shader))
{
}

void IMaterial::SetShader(std::shared_ptr<Shader> shader)
{
	_shader = std::move(shader);
}

