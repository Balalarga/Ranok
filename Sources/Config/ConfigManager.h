#pragma once
#include <cassert>
#include <map>
#include <memory>
#include <string>
#include "IConfig.h"

namespace Ranok
{
class ConfigManager
{
	friend class IConfig;
public:
	static ConfigManager& Instance();
	~ConfigManager() = default;

	template<class T, class ...TArgs>
	std::enable_if_t<std::derived_from<T, IConfig>, std::shared_ptr<T>> CreateConfigs(TArgs&& ...args)
	{
		ConfigData configData;
		configData.userConfig = std::make_shared<T>(args...);
		configData.defaultConfig = std::make_shared<T>(args...);
		assert(!_configs.contains(configData.defaultConfig->GetFilepath()));
		
		_configs[configData.defaultConfig->GetFilepath()] = configData;
		
		LoadConfig(configData);
		if (configData.defaultConfig->IsDefaultOnly())
			return std::static_pointer_cast<T>(configData.defaultConfig);
		return std::static_pointer_cast<T>(configData.userConfig);
	}

	static const std::string& GetConfigDir();
	static const std::string& GetDefaultConfigDir();

	void SaveConfig(IConfig* configs);
	void SaveAll();

	
private:
	struct ConfigData
	{
		std::shared_ptr<IConfig> defaultConfig;
		std::shared_ptr<IConfig> userConfig;
	};
	
	std::map<std::string, ConfigData> _configs;

	static std::string GetFullPath(IConfig* config);
	static std::string GetFullDefaultPath(IConfig* config);
	
	void LoadConfig(ConfigData& configs);
	void LoadDefaultConfig(std::shared_ptr<IConfig>& configs);
	
	void SaveConfig(ConfigData& configs);
	void SaveDefaultConfig(std::shared_ptr<IConfig>& configs);
};
}
