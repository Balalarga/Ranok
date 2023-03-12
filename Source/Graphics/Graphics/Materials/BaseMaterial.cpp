#include "BaseMaterial.h"

#include "Graphics/Shading/ShaderInfo.h"
#include "Graphics/Shading/ShaderStorage.h"

bool vShader = Ranok::ShaderStorage::Instance().AddCppShaderPreloading("baseMatVert", ShaderType::Vertex, "Shaders/BaseMaterial.vert");
bool fShader = Ranok::ShaderStorage::Instance().AddCppShaderPreloading("baseMatFrag", ShaderType::Fragment, "Shaders/BaseMaterial.frag");

BaseMaterial::BaseMaterial(std::shared_ptr<Shader> inShader):
	IMaterial(std::move(inShader)),
	_color(0),
	_mvp(0)
{
}

void BaseMaterial::SetupUniforms()
{
	SetUniform("uColor", _color);
	SetUniform("uMvp", _mvp);
}
