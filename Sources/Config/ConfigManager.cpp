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

ConfigManager::~ConfigManager()
{
}

const std::string& ConfigManager::GetConfigDir()
{
	return CONFIG_DIR;
}

const std::string& ConfigManager::GetDefaultConfigDir()
{
	return CONFIG_DEFAULT_DIR;
}

void ConfigManager::Saveconfig(IConfig* configs)
{
	auto it = _configs.find(configs->GetFilepath());
	if (it == _configs.end())
	{
		Logger::Warning(fmt::format("Try to save configs {} with out adding to ConfigManager", configs->GetFilepath()));
		return;
	}
	if (!configs->IsDefaultOnly() && configs != it->second.defaultconfig.get())
	{
		Saveconfig(configs);
	}
#if OVERRIDE_DEFAULT_CONFIG
	SaveDefaultconfig(it->second.defaultconfig);
#endif
}

void ConfigManager::SaveAll()
{
	for (auto& [_, config]: _configs)
	{
		if (!config.defaultconfig->IsDefaultOnly() &&
			config.userconfig.get() != config.defaultconfig.get())
		{
			Saveconfig(config);
		}
#if OVERRIDE_DEFAULT_CONFIG
		SaveDefaultconfig(config.defaultconfig);
#endif
	}
}

std::string ConfigManager::GetFullPath(IConfig* config)
{
	return CONFIG_DIR"/"+config->GetFilepath();
}

std::string ConfigManager::GetFullDefaultPath(IConfig* config)
{
	return CONFIG_DEFAULT_DIR"/"+config->GetFilepath();
}

void ConfigManager::Loadconfig(ConfigData& configs)
{
	LoadDefaultconfig(configs.defaultconfig);
	if (configs.defaultconfig->IsDefaultOnly())
		return;
	
	std::string path = GetFullPath(configs.userconfig.get());
	if (Files::IsFileExists(path))
	{
		JsonArchive serializer(path, ArchiveMode::Read);
		if (serializer.Open())
			configs.userconfig->Serialize(serializer);
		else
			Logger::Error(fmt::format("Couldn't open {}", path));
	}
}

void ConfigManager::LoadDefaultconfig(std::shared_ptr<IConfig>& config)
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

void ConfigManager::Saveconfig(ConfigData& config)
{
	std::string path = GetFullPath(config.userconfig.get());
	JsonArchive serializer(path, ArchiveMode::Write);
	if (serializer.Open())
		config.userconfig->Serialize(serializer);
	else
		Logger::Error(fmt::format("Couldn't open {}", path));
}

void ConfigManager::SaveDefaultconfig(std::shared_ptr<IConfig>& config)
{
	std::string path = GetFullDefaultPath(config.get());
	JsonArchive serializer(path, ArchiveMode::Write);
	if (serializer.Open())
		config->Serialize(serializer);
	else
		Logger::Error(fmt::format("Couldn't open {}", path));
}
}
