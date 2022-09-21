#include "Logger.h"

namespace Ranok
{
std::vector<std::unique_ptr<IOutputDevice>> Logger::devices = {};
}
