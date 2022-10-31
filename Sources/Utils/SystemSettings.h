#pragma once
#include "Config/IConfig.h"

namespace Ranok
{
class SystemSettings: public IConfig
{
public:
	SystemSettings();
	void Serialize(JsonArchive& archive) override;

	const std::string resourcesDir;
	const std::string configsDir;
	const std::string defaultConfigsDir;
	const std::string defaultLibrariesDir;
};
}