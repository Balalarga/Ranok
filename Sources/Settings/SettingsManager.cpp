#include "SettingsManager.h"
#include "Settings/ISettings.h"

#include "Utils/FileUtils.h"
#include "Utils/Archives/Json/JsonArchive.h"

namespace Ranok
{

SettingsManager& SettingsManager::Instance()
{
	static SettingsManager manager;
	return manager;
}

SettingsManager::~SettingsManager()
{
}

const std::string& SettingsManager::GetConfigDir()
{
	return CONFIG_DIR;
}

const std::string& SettingsManager::GetDefaultConfigDir()
{
	return CONFIG_DEFAULT_DIR;
}

void SettingsManager::SaveSetting(ISettings* settings)
{
	auto it = _settings.find(settings->GetFilepath());
	if (it == _settings.end())
	{
		Logger::Warning(fmt::format("Try to save settings {} with out adding to SettingsManager", settings->GetFilepath()));
		return;
	}
	if (!settings->IsDefaultOnly() && settings != it->second.defaultSetting.get())
	{
		SaveSetting(settings);
	}
#if OVERRIDE_DEFAULT_CONFIG
	SaveDefaultSetting(it->second.defaultSetting);
#endif
}

void SettingsManager::SaveAll()
{
	for (auto& [_, setting]: _settings)
	{
		if (!setting.defaultSetting->IsDefaultOnly() &&
			setting.userSetting.get() != setting.defaultSetting.get())
		{
			SaveSetting(setting);
		}
#if OVERRIDE_DEFAULT_CONFIG
		SaveDefaultSetting(setting.defaultSetting);
#endif
	}
}

std::string SettingsManager::GetFullPath(ISettings* setting)
{
	return CONFIG_DIR"/"+setting->GetFilepath();
}

std::string SettingsManager::GetFullDefaultPath(ISettings* setting)
{
	return CONFIG_DEFAULT_DIR"/"+setting->GetFilepath();
}

void SettingsManager::LoadSetting(SettingData& settings)
{
	LoadDefaultSetting(settings.defaultSetting);
	if (settings.defaultSetting->IsDefaultOnly())
		return;
	
	std::string path = GetFullPath(settings.userSetting.get());
	if (Files::IsFileExists(path))
	{
		JsonArchive serializer(path, ArchiveMode::Read);
		if (serializer.Open())
			settings.userSetting->Serialize(serializer);
		else
			Logger::Error(fmt::format("Couldn't open {}", path));
	}
}

void SettingsManager::LoadDefaultSetting(std::shared_ptr<ISettings>& setting)
{
	std::string path = GetFullDefaultPath(setting.get());
	if (Files::IsFileExists(path))
	{
		JsonArchive serializer(path, ArchiveMode::Read);
		if (serializer.Open())
			setting->Serialize(serializer);
		else
			Logger::Error(fmt::format("Couldn't open {}", path));
	}
}

void SettingsManager::SaveSetting(SettingData& setting)
{
	std::string path = GetFullPath(setting.userSetting.get());
	JsonArchive serializer(path, ArchiveMode::Write);
	if (serializer.Open())
		setting.userSetting->Serialize(serializer);
	else
		Logger::Error(fmt::format("Couldn't open {}", path));
}

void SettingsManager::SaveDefaultSetting(std::shared_ptr<ISettings>& setting)
{
	std::string path = GetFullDefaultPath(setting.get());
	JsonArchive serializer(path, ArchiveMode::Write);
	if (serializer.Open())
		setting->Serialize(serializer);
	else
		Logger::Error(fmt::format("Couldn't open {}", path));
}
}
