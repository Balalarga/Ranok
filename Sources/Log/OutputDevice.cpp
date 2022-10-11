#include "OutputDevice.h"
#include <iostream>

namespace Ranok
{
void IOutputDevice::SetMinLogLevel(LogLevel level)
{
    _minLevel = level;
}

void CmdOutput::Write(const LogLevel& level, std::string &&text)
{
    std::cout << text << std::endl;
}
}
