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
	~ConfigManager();

	template<class T, class ...TArgs>
	std::enable_if_t<std::derived_from<T, IConfig>, std::shared_ptr<T>> Createconfigs(TArgs&& ...args)
	{
		ConfigData configData;
		configData.userconfig = std::make_shared<T>(args...);
		configData.defaultconfig = std::make_shared<T>(args...);
		assert(!_configs.contains(configData.defaultconfig->GetFilepath()));
		
		_configs[configData.defaultconfig->GetFilepath()] = configData;
		
		Loadconfig(configData);
		if (configData.defaultconfig->IsDefaultOnly())
			return std::static_pointer_cast<T>(configData.defaultconfig);
		return std::static_pointer_cast<T>(configData.userconfig);
	}

	static const std::string& GetConfigDir();
	static const std::string& GetDefaultConfigDir();

	void Saveconfig(IConfig* configs);
	void SaveAll();

	
private:
	struct ConfigData
	{
		std::shared_ptr<IConfig> defaultconfig;
		std::shared_ptr<IConfig> userconfig;
	};
	
	std::map<std::string, ConfigData> _configs;

	static std::string GetFullPath(IConfig* config);
	static std::string GetFullDefaultPath(IConfig* config);
	
	void Loadconfig(ConfigData& configs);
	void LoadDefaultconfig(std::shared_ptr<IConfig>& configs);
	
	void Saveconfig(ConfigData& configs);
	void SaveDefaultconfig(std::shared_ptr<IConfig>& configs);
};
}
