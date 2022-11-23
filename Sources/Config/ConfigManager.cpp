#include "ConfigManager.h"
#include "Config/IConfig.h"
#include "Utils/FileUtils.h"
#include "Utils/Archives/Json/JsonArchive.h"

namespace Ranok
{

ConfigManager& ConfigManager::Instance()
{
	static ConfigManager manager;
	return manager;
}

std::string ConfigManager::GetConfigDir()
{
#if defined(_DEBUG) || defined(DEBUG)
	return "../Config";
#else
	return "./Config";
#endif
}

std::string ConfigManager::GetDefaultConfigDir()
{
#if defined(_DEBUG) || defined(DEBUG)
	return PROJECT_SOURCE_DIR"/Assets/DefaultConfig";
#else
	return "Assets/DefaultConfig";
#endif
}

void ConfigManager::SaveAll()
{
	for (auto& config : _configs | std::views::values)
	{
		if (!config.defaultConfig->IsDefaultOnly() && *config.defaultConfig != *config.userConfig)
			SaveConfig(config);
#if defined(_DEBUG) || defined(DEBUG) 
		SaveDefaultConfig(config.defaultConfig);
#endif
	}
}

std::map<std::string, std::shared_ptr<IConfig>> ConfigManager::GetConfigs()
{
	std::map<std::string, std::shared_ptr<IConfig>> result;
	for (auto& [path, config] : _configs)
	{
		size_t nameStart = path.find_last_of('/');
		if (nameStart ==  std::string::npos)
			nameStart = path.find_last_of('\\');
		if (nameStart ==  std::string::npos)
			nameStart = 0;
		
		const std::string name = path.substr(nameStart);
		if (config.defaultConfig->IsDefaultOnly())
			result.insert({name, config.defaultConfig});
		else
			result.insert({name, config.userConfig});
	}
	return result;
}

std::string ConfigManager::GetFullPath(IConfig* config)
{
	return GetConfigDir()+"/"+config->GetFilepath();
}

std::string ConfigManager::GetFullDefaultPath(IConfig* config)
{
#ifdef _DEBUG
	return PROJECT_SOURCE_DIR"/Assets/DefaultConfig/"+config->GetFilepath();
#else
	return "Assets/DefaultConfig/"+config->GetFilepath();
#endif
}

void ConfigManager::LoadConfig(ConfigData& configs) const
{
	LoadDefaultConfig(configs.defaultConfig);
	if (configs.defaultConfig->IsDefaultOnly())
		return;
	
	std::string path = GetFullPath(configs.userConfig.get());
	if (Files::IsFileExists(path))
	{
		JsonArchive serializer(path, ArchiveMode::Read);
		if (serializer.Open())
			configs.userConfig->Serialize(serializer);
		else
			Logger::Error(fmt::format("Couldn't open {}", path));
	}
}

void ConfigManager::LoadDefaultConfig(std::shared_ptr<IConfig>& config) const
{
	std::string path = GetFullDefaultPath(config.get());
	if (Files::IsFileExists(path))
	{
		JsonArchive serializer(path, ArchiveMode::Read);
		if (serializer.Open())
			config->Serialize(serializer);
		else
			Logger::Error(fmt::format("Couldn't open {}", path));
	}
}

void ConfigManager::SaveConfig(ConfigData& config) const
{
	std::string path = GetFullPath(config.userConfig.get());
	JsonArchive serializer(path, ArchiveMode::Write);
	if (serializer.Open())
		config.userConfig->Serialize(serializer);
	else
		Logger::Error(fmt::format("Couldn't open {}", path));
}

void ConfigManager::SaveDefaultConfig(std::shared_ptr<IConfig>& config) const
{
	std::string path = GetFullDefaultPath(config.get());
	JsonArchive serializer(path, ArchiveMode::Write);
	if (serializer.Open())
		config->Serialize(serializer);
	else
		Logger::Error(fmt::format("Couldn't open {}", path));
}
}
