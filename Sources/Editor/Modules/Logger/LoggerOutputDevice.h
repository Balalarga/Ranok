#pragma once
#include "Log/OutputDevice.h"

namespace Ranok
{
class LoggerModule;

class LoggerOutputDevice: public IOutputDevice
{
public:
	LoggerOutputDevice(LoggerModule& logger);
	
	void Write(std::string &&text) override;
	
private:
	LoggerModule& _logger;
};
}
