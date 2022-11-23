#pragma once

#include <glm/glm.hpp>

#include "Graphics/Buffers/Buffer.h"
#include "Graphics/Buffers/FrameBuffer.h"
#include "Graphics/Materials/IMaterial.h"
#include "Graphics/Rendering/Object.h"

#include "Language/Generators/ShaderGenerator.h"

class Scene;

namespace Ranok
{
class RaymarchingMaterial: public IMaterial
{
public:
	RaymarchingMaterial(const std::shared_ptr<Shader>& shader);
	void SetupUniforms() override;
};


class RayMarchingView: public FrameBuffer, public Object
{
public:
	RayMarchingView();
	std::optional<std::string> SetProgram(ActionTree& tree);

	void Bind() override;

private:
	RaymarchingMaterial _material;
	
	static glm::fvec2 vertices[6];
	static Buffer bufferInfo;
	static std::string shaderHeader;
	static std::string shaderFooter;
	static std::string defaultFragmentShader;
	static std::string defaultVertexShader;
	
	static ShaderGenerator _codeGenerator;
	static std::shared_ptr<ShaderPart> _defaultVShader;
	static std::shared_ptr<ShaderPart> _defaultFShader;
	static std::shared_ptr<Shader> _defaultShader;
};
}