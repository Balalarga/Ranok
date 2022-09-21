#include "OutputDevice.h"
#include <iostream>

namespace Ranok
{
void CmdOutput::Write(std::string &&text)
{
    std::cout << text;
}
}
