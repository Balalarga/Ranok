#pragma once
#include <map>
#include <memory>
#include <string>

#include "ShaderPart.h"
#include "Uniform.h"


class ShaderPart;

class Shader
{
public:
	Shader(std::shared_ptr<ShaderPart> vShader,
	       std::shared_ptr<ShaderPart> fShader,
	       std::shared_ptr<ShaderPart> gShader = nullptr);
	~Shader();

	bool Compile();
	void Destroy();

	bool UpdateShaderPart(std::shared_ptr<ShaderPart> part);
	
	void Bind();
	static void Release();

	bool SetUniform(const std::string& name, const UniformValue& value, bool bUnbind = false);

	bool IsCompiled() const { return _glHandler != 0; }
	
protected:
	int GetUniformLocation(const std::string& name);

private:
	struct
	{
		std::shared_ptr<ShaderPart> vShader;
		std::shared_ptr<ShaderPart> fShader;
		std::shared_ptr<ShaderPart> gShader;
	} _parts;

	unsigned _glHandler = 0;

	std::map<std::string, int> _uniformCache;
};
