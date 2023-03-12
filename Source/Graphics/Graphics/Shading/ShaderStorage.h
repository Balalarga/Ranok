#pragma once

#include <map>
#include <string>
#include <vector>

#include "ShaderInfo.h"
#include "ShaderPart.h"

namespace Ranok
{
class ShaderStorage
{
public:
	static ShaderStorage& Instance();
	
	std::shared_ptr<Shader> LoadShader(ShaderInfo& info);
	std::shared_ptr<Shader> FindShader(const std::string& tag);
	
	std::vector<ShaderInfo*> CompileAll();
	void Preloading();
	
	bool AddCppShaderPreloading(const std::string& tag, const ShaderType& type, const std::string& filepath);
	
	
private:
	ShaderStorage() = default;
	std::map<std::string, ShaderInfo> _shaders;
};
}
