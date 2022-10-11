#pragma once
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
	std::enable_if_t<std::derived_from<T, ISettings>, std::shared_ptr<T>> AddSettings(TArgs&& ...args)
	{
		std::shared_ptr<ISettings> setting = std::make_shared<T>(args...);
		auto it = _settings.find(setting->GetFilepath());
		if (it != _settings.end())
			return std::static_pointer_cast<T>(it->second);
		
		_settings[setting->GetFilepath()] = setting;
		
		if (_bWasLoaded)
			LoadSetting(setting.get());
		
		return std::static_pointer_cast<T>(setting);
	}

	void SetConfigDir(const std::string& configDir);
	void SetDefaultConfigDir(const std::string& configDir);

	const std::string& GetConfigDir() const { return _configDir; }
	const std::string& GetDefaultConfigDir() const { return _defaultConfigDir; }

	void LoadAll();
	void SaveAll();
	
	
private:
	std::string _defaultConfigDir = "Configs";
	std::string _configDir = "E:\\Projects\\Cpp\\Ranok\\VsProject";
	bool _bWasLoaded = false;
	
	std::map<std::string, std::shared_ptr<ISettings>> _settings;

	std::string GetFullPath(ISettings* setting);
	std::string GetFullDefaultPath(ISettings* setting);
	
	void LoadSetting(ISettings* settings);
	void LoadDefaultSetting(ISettings* settings);
	
	void SaveSetting(ISettings* settings);
};
}
