#pragma once
#include <memory>
#include <string>

class Shader;

namespace Ranok
{
struct ShaderInfo
{
	std::string tag;
	std::string vFilepath;
	std::string fFilepath;
	std::shared_ptr<Shader> shader;
};
}
