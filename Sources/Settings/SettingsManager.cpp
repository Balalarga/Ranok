#include "SettingsManager.h"
#include "Settings/ISettings.h"

#include "Utils/FileUtils.h"
#include "Utils/Archives/Json/JsonArchive.h"

namespace Ranok
{

std::string _sDefaultConfigDir = CONFIG_DEFAULT_DIR;
std::string _sConfigDir = CONFIG_DIR;

SettingsManager& SettingsManager::Instance()
{
	static SettingsManager manager;
	return manager;
}

const std::string& SettingsManager::GetConfigDir()
{
	return _sConfigDir;
}

const std::string& SettingsManager::GetDefaultConfigDir()
{
	return _sDefaultConfigDir;
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
	return _sConfigDir+"/"+setting->GetFilepath();
}

std::string SettingsManager::GetFullDefaultPath(ISettings* setting)
{
	return _sDefaultConfigDir+"/"+setting->GetFilepath();
}

void SettingsManager::LoadSetting(SettingData& settings)
{
	LoadDefaultSetting(settings.defaultSetting);
	std::string path = GetFullPath(settings.userSetting.get());
	if (Files::IsFileExists(path))
	{
		JsonArchive serializer(path, ArchiveMode::Read);
		settings.userSetting->Serialize(serializer);
	}
}

void SettingsManager::LoadDefaultSetting(std::shared_ptr<ISettings>& setting)
{
	std::string path = GetFullDefaultPath(setting.get());
	if (Files::IsFileExists(path))
	{
		JsonArchive serializer(path, ArchiveMode::Read);
		setting->Serialize(serializer);
	}
}

void SettingsManager::SaveSetting(SettingData& setting)
{
	std::string path = GetFullPath(setting.userSetting.get());
	JsonArchive serializer(path, ArchiveMode::Write);
	serializer.Open();
	setting.userSetting->Serialize(serializer);
	serializer.Close();
	Logger::Log(path);
}

void SettingsManager::SaveDefaultSetting(std::shared_ptr<ISettings>& setting)
{
	std::string path = GetFullPath(setting.get());
	JsonArchive serializer(path, ArchiveMode::Write);
	serializer.Open();
	setting->Serialize(serializer);
	serializer.Close();
	Logger::Log(path);
}
}
