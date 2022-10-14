#include "ISettings.h"

namespace Ranok
{
ISettings::ISettings(const std::string& filepath, bool bDefaultOnly):
	_filepath(filepath + ".json"),
	_bDefaultOnly(bDefaultOnly)
{
}
}
