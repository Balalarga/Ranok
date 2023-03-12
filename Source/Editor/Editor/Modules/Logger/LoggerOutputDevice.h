#pragma once
#include "Log/LogOutputDevice.h"

namespace Ranok
{
class LoggerModule;

class LoggerOutputDevice: public ILogOutputDevice
{
public:
	LoggerOutputDevice(LoggerModule& logger);
	
	void Write(const LogLevel& level, std::string &&text) override;
	
private:
	LoggerModule& _logger;
};
}
