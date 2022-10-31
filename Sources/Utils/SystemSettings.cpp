#include "SystemSettings.h"

#include "Archives/Json/JsonArchive.h"

namespace Ranok
{
SystemSettings::SystemSettings():
	IConfig("SystemSettings", true)
{
}

void SystemSettings::Serialize(JsonArchive& archive)
{
	archive.Serialize("resourcesDir", resourcesDir);
	archive.Serialize("configsDir", configsDir);
	archive.Serialize("defaultConfigsDir", defaultConfigsDir);
	archive.Serialize("defaultLibrariesDir", defaultLibrariesDir);
}
}
