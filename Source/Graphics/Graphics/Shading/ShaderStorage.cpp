#include "ShaderStorage.h"

#include <ranges>

#include "Shader.h"

#include "Config/ConfigManager.h"
#include "Config/IConfig.h"

#include "Utils/FileUtils.h"
#include "Utils/Archives/Json/JsonArchive.h"

namespace Ranok
{
struct ShaderStorageConfigItemData
{
	std::string tag;
	ShaderType type;
	std::string assetFilepath;
};
void to_json(nlohmann::json& j, const ShaderStorageConfigItemData& p)
{
	j = nlohmann::json{ {"tag", p.tag}, {"type", p.type}, {"assetFilepath", p.assetFilepath} };
}

void from_json(const nlohmann::json& j, ShaderStorageConfigItemData& p)
{
	j.at("tag").get_to(p.tag);
	j.at("type").get_to(p.type);
	j.at("assetFilepath").get_to(p.assetFilepath);
}

class ShaderStorageConfig: public IConfig
{
public:
	
	ShaderStorageConfig():
		IConfig("Shaders/ShaderStorage", true)
	{
		SetNotLoadable(true);
	}

	void Serialize(JsonArchive& archive) override
	{
		archive.Serialize("preloadingShaders", preloadingShaders);
	}
	
	bool operator!=(const IConfig& oth) override
	{
		const auto casted = dynamic_cast<const ShaderStorageConfig*>(&oth);
		assert(casted);
		return true;
	}
	std::vector<ShaderStorageConfigItemData> preloadingShaders;
};

static std::shared_ptr<ShaderStorageConfig> GetConfig()
{
	static std::shared_ptr<ShaderStorageConfig> config = ConfigManager::Instance().CreateConfigs<ShaderStorageConfig>();
	return config;
}

ShaderStorage& ShaderStorage::Instance()
{
	static ShaderStorage storage;
	return storage;
}

std::shared_ptr<Shader> ShaderStorage::LoadShader(ShaderInfo& info)
{
	if (std::shared_ptr<Shader> shader = FindShader(info.tag))
		return shader;

	std::optional<std::string> vData = Files::ReadFile(info.vFilepath);
	std::optional<std::string> fData = Files::ReadFile(info.fFilepath);
	if (!vData.has_value() || !fData.has_value())
		return {};

	info.shader = std::make_shared<Shader>(
			std::make_shared<ShaderPart>(ShaderType::Vertex, vData.value()),
			std::make_shared<ShaderPart>(ShaderType::Fragment, fData.value())
		);
	_shaders[info.tag] = info;
	return info.shader;
}

std::shared_ptr<Shader> ShaderStorage::FindShader(const std::string& tag)
{
	auto it = _shaders.find(tag);
	if (it != _shaders.end())
		return it->second.shader;

	return nullptr;
}

std::vector<ShaderInfo*> ShaderStorage::CompileAll()
{
	std::vector<ShaderInfo*> status;

	for (ShaderInfo& shaderInfo : _shaders | std::ranges::views::values)
	{
		// if (!shaderInfo.part->Compile())
			// status.push_back(&shaderInfo);
	}
	
	return status;
}

void ShaderStorage::Preloading()
{
	// for (ShaderStorageConfigItemData& item : config->preloadingShaders)
		// LoadShader({item.tag, item.type, Files::GetAssetPath(item.assetFilepath)});
}

bool ShaderStorage::AddCppShaderPreloading(const std::string& tag, const ShaderType& type, const std::string& filepath)
{
	for (ShaderStorageConfigItemData& item : GetConfig()->preloadingShaders)
	{
		if (item.tag == tag)
			return false;
	}
	
	GetConfig()->preloadingShaders.push_back({tag, type, filepath});
	return true;
}
}
