#pragma once
#include <cassert>
#include <map>
#include <memory>
#include <string>
#include "ISettings.h"

namespace Ranok
{
class SettingsManager
{
	friend class ISettings;
public:
	static SettingsManager& Instance();
	~SettingsManager();

	template<class T, class ...TArgs>
	std::enable_if_t<std::derived_from<T, ISettings>, std::shared_ptr<T>> CreateSettings(TArgs&& ...args)
	{
		SettingData settingData;
		settingData.userSetting = std::make_shared<T>(args...);
		settingData.defaultSetting = std::make_shared<T>(args...);
		assert(!_settings.contains(settingData.defaultSetting->GetFilepath()));
		
		_settings[settingData.defaultSetting->GetFilepath()] = settingData;
		
		LoadSetting(settingData);
		if (settingData.defaultSetting->IsDefaultOnly())
			return std::static_pointer_cast<T>(settingData.defaultSetting);
		return std::static_pointer_cast<T>(settingData.userSetting);
	}

	static const std::string& GetConfigDir();
	static const std::string& GetDefaultConfigDir();

	void SaveSetting(ISettings* settings);
	void SaveAll();

	
private:
	struct SettingData
	{
		std::shared_ptr<ISettings> defaultSetting;
		std::shared_ptr<ISettings> userSetting;
	};
	
	std::map<std::string, SettingData> _settings;

	static std::string GetFullPath(ISettings* setting);
	static std::string GetFullDefaultPath(ISettings* setting);
	
	void LoadSetting(SettingData& settings);
	void LoadDefaultSetting(std::shared_ptr<ISettings>& settings);
	
	void SaveSetting(SettingData& settings);
	void SaveDefaultSetting(std::shared_ptr<ISettings>& settings);
};
}
