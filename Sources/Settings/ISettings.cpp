#include "ISettings.h"

#include "SettingsManager.h"

namespace Ranok
{
ISettings::ISettings(const std::string& filepath):
	_filepath(filepath)
{
}
}
