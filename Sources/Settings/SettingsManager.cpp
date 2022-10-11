#include "SettingsManager.h"
#include "Settings/ISettings.h"

#include "Utils/FileUtils.h"
#include "Utils/Archives/Serializer.h"
#include "Utils/Archives/Json/JsonArchive.h"

namespace Ranok
{
SettingsManager& SettingsManager::Instance()
{
	static SettingsManager manager;
	return manager;
}

void SettingsManager::SetConfigDir(const std::string& configDir)
{
	_configDir = configDir;
}

void SettingsManager::SetDefaultConfigDir(const std::string& configDir)
{
	_defaultConfigDir = configDir;
}

void SettingsManager::LoadAll()
{
	for (auto& [filepath, setting]: _settings)
		LoadSetting(setting.get());
	
	_bWasLoaded = true;
}

void SettingsManager::SaveAll()
{
	for (auto& [filepath, setting]: _settings)
		SaveSetting(setting.get());
}

std::string SettingsManager::GetFullPath(ISettings* setting)
{
	return _configDir+"/"+setting->GetFilepath();
}

std::string SettingsManager::GetFullDefaultPath(ISettings* setting)
{
	return _defaultConfigDir+"/"+setting->GetFilepath();
}

void SettingsManager::LoadSetting(ISettings* setting)
{
	LoadDefaultSetting(setting);
	std::string path = GetFullPath(setting);
	if (Files::IsFileExists(path))
	{
		Serializer serializer = Serializer::LoadFrom<JsonArchiveReader>(path);
		setting->Serialize(serializer);
	}
}

void SettingsManager::LoadDefaultSetting(ISettings* setting)
{
	std::string path = GetFullDefaultPath(setting);
	if (Files::IsFileExists(path))
	{
		Serializer serializer = Serializer::LoadFrom<JsonArchiveReader>(path);
		setting->Serialize(serializer);
	}
}

void SettingsManager::SaveSetting(ISettings* setting)
{
	std::string path = GetFullPath(setting);
	Serializer serializer = Serializer::SaveTo<JsonArchiveWriter>(path);
	setting->Serialize(serializer);
	Logger::Log(path);
}
}
