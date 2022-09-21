#pragma once

#include <string>

namespace Ranok
{
class IOutputDevice
{
public:
    virtual ~IOutputDevice() = default;
    virtual void Write(std::string&& text) = 0;
};

class CmdOutput: public IOutputDevice
{
public:
    void Write(std::string &&text) override;
};
}
