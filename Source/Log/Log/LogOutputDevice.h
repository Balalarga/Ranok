#pragma once

#include <string>
#include "Logger.h"

namespace Ranok
{
class ILogOutputDevice
{
public:
    virtual ~ILogOutputDevice() = default;
    virtual void Write(const LogLevel& level, std::string&& text) = 0;
    
    const LogLevel& GetMinLogLevel() const { return _minLevel; }
    void SetMinLogLevel(LogLevel level);
    
private:
    LogLevel _minLevel = LogLevel::Verbose;
};


class CmdOutput: public ILogOutputDevice
{
public:
    void Write(const LogLevel& level, std::string &&text) override;
};
}
