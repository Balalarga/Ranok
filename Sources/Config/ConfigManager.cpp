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

const std::string& ConfigManager::GetConfigDir()
{
	return SystemSettings::configsDir;
}

const std::string& ConfigManager::GetDefaultConfigDir()
{
	
#ifdef _DEBUG
	return PROJECT_SOURCE_DIR"\\Assets\\DefaultConfig";
#else
	return ".\\Assets\\DefaultConfig";
#endif
}

void ConfigManager::SaveConfig(IConfig* configs)
{
	auto it = _configs.find(configs->GetFilepath());
	if (it == _configs.end())
	{
		Logger::Warning(fmt::format("Try to save configs {} with out adding to ConfigManager", configs->GetFilepath()));
		return;
	}
	if (!configs->IsDefaultOnly() && configs != it->second.defaultConfig.get())
	{
		SaveConfig(configs);
	}
#if OVERRIDE_DEFAULT_CONFIG
	SaveDefaultConfig(it->second.defaultConfig);
#endif
}

void ConfigManager::SaveAll()
{
	for (auto& [_, config]: _configs)
	{
		if (!config.defaultConfig->IsDefaultOnly() &&
			config.userConfig.get() != config.defaultConfig.get())
		{
			SaveConfig(config);
		}
#if OVERRIDE_DEFAULT_CONFIG
		SaveDefaultConfig(config.defaultConfig);
#endif
	}
}

std::string ConfigManager::GetFullPath(IConfig* config)
{
	return GetConfigDir()+"/"+config->GetFilepath();
}

std::string ConfigManager::GetFullDefaultPath(IConfig* config)
{
	
#ifdef _DEBUG
	return PROJECT_SOURCE_DIR"\\Assets\\DefaultConfig\\"+config->GetFilepath();
#else
	return ".\\Assets\\DefaultConfig\\"+config->GetFilepath();
#endif
}

void ConfigManager::LoadConfig(ConfigData& configs)
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

void ConfigManager::LoadDefaultConfig(std::shared_ptr<IConfig>& config)
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

void ConfigManager::SaveConfig(ConfigData& config)
{
	std::string path = GetFullPath(config.userConfig.get());
	JsonArchive serializer(path, ArchiveMode::Write);
	if (serializer.Open())
		config.userConfig->Serialize(serializer);
	else
		Logger::Error(fmt::format("Couldn't open {}", path));
}

void ConfigManager::SaveDefaultConfig(std::shared_ptr<IConfig>& config)
{
	std::string path = GetFullDefaultPath(config.get());
	JsonArchive serializer(path, ArchiveMode::Write);
	if (serializer.Open())
		config->Serialize(serializer);
	else
		Logger::Error(fmt::format("Couldn't open {}", path));
}
}
