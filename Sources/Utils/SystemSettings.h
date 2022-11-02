#pragma once

#include "Config/IConfig.h"

namespace Ranok
{
class SystemSettings
{
public:
	static std::string resourcesDir;
	static std::string configsDir;
	static std::string defaultConfigsDir;
	static std::string defaultLibrariesDir;
};
}