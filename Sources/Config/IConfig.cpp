#include "IConfig.h"

namespace Ranok
{
IConfig::IConfig(const std::string& filepath, bool bDefaultOnly, bool bHasGui):
	_filepath(filepath + ".json"),
	_bDefaultOnly(bDefaultOnly),
	_bHaveGui(bHasGui)
{
}
}