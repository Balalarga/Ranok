#pragma once
#include "IMaterial.h"

class BaseMaterial: public IMaterial
{
public:
	BaseMaterial(std::shared_ptr<Shader> inShader = nullptr);
	
	void SetupUniforms() override;
	
	void SetColor(const glm::vec4& color) { _color = color; }
	void SetMvp(const glm::mat4& mvp) { _mvp = mvp; }
	
private:
	glm::vec4 _color;
	glm::mat4 _mvp;
};
