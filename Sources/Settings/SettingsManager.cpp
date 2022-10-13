#include "SettingsManager.h"
#include "Settings/ISettings.h"

#include "Utils/FileUtils.h"
#include "Utils/Archives/Serializer.h"
#include "Utils/Archives/Json/JsonArchive.h"

namespace Ranok
{

std::string _defaultConfigDir = CONFIG_DEFAULT_DIR;
std::string _configDir = CONFIG_DIR;

SettingsManager& SettingsManager::Instance()
{
	static SettingsManager manager;
	return manager;
}

const std::string& SettingsManager::GetConfigDir()
{
	return _configDir;
}

const std::string& SettingsManager::GetDefaultConfigDir()
{
	return _defaultConfigDir;
}

void SettingsManager::LoadAll()
{
	for (auto& [_, setting]: _settings)
		LoadSetting(setting);
	
	_bWasLoaded = true;
}

void SettingsManager::SaveAll()
{
	for (auto& [_, setting]: _settings)
		SaveSetting(setting);
}

std::string SettingsManager::GetFullPath(ISettings* setting)
{
	return _configDir+"/"+setting->GetFilepath();
}

std::string SettingsManager::GetFullDefaultPath(ISettings* setting)
{
	return _defaultConfigDir+"/"+setting->GetFilepath();
}

void SettingsManager::LoadSetting(SettingData& settings)
{
	LoadDefaultSetting(settings.defaultSetting);
	std::string path = GetFullPath(settings.userSetting.get());
	if (Files::IsFileExists(path))
	{
		Serializer serializer = Serializer::LoadFrom<JsonArchiveReader>(path);
		settings.userSetting->Serialize(serializer);
	}
}

void SettingsManager::LoadDefaultSetting(std::shared_ptr<ISettings>& setting)
{
	std::string path = GetFullDefaultPath(setting.get());
	if (Files::IsFileExists(path))
	{
		Serializer serializer = Serializer::LoadFrom<JsonArchiveReader>(path);
		setting->Serialize(serializer);
	}
}

void SettingsManager::SaveSetting(SettingData& setting)
{
	std::string path = GetFullPath(setting.userSetting.get());
	Serializer serializer = Serializer::SaveTo<JsonArchiveWriter>(path);
	setting.userSetting->Serialize(serializer);
	Logger::Log(path);
}

void SettingsManager::SaveDefaultSetting(std::shared_ptr<ISettings>& setting)
{
	std::string path = GetFullPath(setting.get());
	Serializer serializer = Serializer::SaveTo<JsonArchiveWriter>(path);
	setting->Serialize(serializer);
	Logger::Log(path);
}
}
