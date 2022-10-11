#pragma once

#include <string>
#include "Logger.h"

namespace Ranok
{
class IOutputDevice
{
public:
    virtual ~IOutputDevice() = default;
    virtual void Write(const LogLevel& level, std::string&& text) = 0;
    
    const LogLevel& GetMinLogLevel() const { return _minLevel; }
    
protected:
    void SetMinLogLevel(LogLevel level);
    
private:
    LogLevel _minLevel = LogLevel::Verbose;
};


class CmdOutput: public IOutputDevice
{
public:
    void Write(const LogLevel& level, std::string &&text) override;
};
}
