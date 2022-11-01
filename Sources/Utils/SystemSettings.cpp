#include "SystemSettings.h"

namespace Ranok
{
#ifdef _DEBUG
std::string SystemSettings::resourcesDir = PROJECT_SOURCE_DIR"\\Assets";
std::string SystemSettings::configsDir = "..\\Config";
std::string SystemSettings::defaultConfigsDir = PROJECT_SOURCE_DIR"\\Assets\\DefaultConfig";
std::string SystemSettings::defaultLibrariesDir = PROJECT_SOURCE_DIR"\\Assets\\CodeLibrary";
#else
std::string SystemSettings::resourcesDir = ".\\Assets";
std::string SystemSettings::configsDir = ".\\Config";
std::string SystemSettings::defaultConfigsDir = ".\\Assets\\DefaultConfig";
std::string SystemSettings::defaultLibrariesDir = ".\\Assets\\CodeLibrary";
#endif
}
