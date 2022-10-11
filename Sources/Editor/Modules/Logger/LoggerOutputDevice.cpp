#include "LoggerOutputDevice.h"
#include "LoggerModule.h"

namespace Ranok
{
LoggerOutputDevice::LoggerOutputDevice(LoggerModule& logger):
	_logger(logger)
{
	SetMinLogLevel(LogLevel::Log);
}

void LoggerOutputDevice::Write(const LogLevel& level, std::string &&text)
{
	_logger.AddLog(text);
}
}
