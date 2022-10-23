#include "IConfig.h"

namespace Ranok
{
IConfig::IConfig(const std::string& filepath, bool bDefaultOnly):
	_filepath(filepath + ".json"),
	_bDefaultOnly(bDefaultOnly)
{
}
}
