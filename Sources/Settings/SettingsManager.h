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

	template<class T, class ...TArgs>
	std::enable_if_t<std::derived_from<T, ISettings>, std::shared_ptr<T>> CreateSettings(TArgs&& ...args)
	{
		std::shared_ptr<T> setting = std::make_shared<T>(args...);
		assert(!_settings.contains(setting->GetFilepath()));
		
		_settings[setting->GetFilepath()] = {std::make_shared<T>(args...), setting};
		
		if (_bWasLoaded)
			LoadSetting(setting.get());
		
		return std::static_pointer_cast<T>(setting);
	}

	static const std::string& GetConfigDir();
	static const std::string& GetDefaultConfigDir();

	void LoadAll();
	void SaveAll();
	
	
private:
	struct SettingData
	{
		std::shared_ptr<ISettings> defaultSetting;
		std::shared_ptr<ISettings> userSetting;
	};
	
	bool _bWasLoaded = false;
	
	std::map<std::string, SettingData> _settings;

	std::string GetFullPath(ISettings* setting);
	std::string GetFullDefaultPath(ISettings* setting);
	
	void LoadSetting(SettingData& settings);
	void LoadDefaultSetting(std::shared_ptr<ISettings>& settings);
	
	void SaveSetting(SettingData& settings);
	void SaveDefaultSetting(std::shared_ptr<ISettings>& settings);
};
}
