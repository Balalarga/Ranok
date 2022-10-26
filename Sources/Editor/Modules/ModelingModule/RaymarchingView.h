#pragma once

#include <glm/glm.hpp>

#include "Language/Generators/ShaderGenerator.h"
#include "OpenGL/Core/Buffer.h"
#include "OpenGL/Core/FrameBuffer.h"
#include "OpenGL/Core/IRenderable.h"

class Scene;

namespace Ranok
{
class RayMarchingView: public IRenderable
{
public:
	RayMarchingView(FrameBuffer& parent);
	bool SetModel(ActionTree& function);

	void Render() override;


private:
	FrameBuffer& _parent;
	ShaderPart _vPart;
	ShaderPart _fPart;
	Shader _shader;
	
	static glm::fvec2 vertices[6];
	static Buffer bufferInfo;
	static std::string shaderHeader;
	static std::string shaderFooter;
	static std::string defaultFragmentShader;
	static std::string defaultVertexShader;

	static ShaderGenerator _codeGenerator;
};
}