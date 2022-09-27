#include "LoggerOutputDevice.h"
#include "LoggerModule.h"

namespace Ranok
{
LoggerOutputDevice::LoggerOutputDevice(LoggerModule& logger):
	_logger(logger)
{}

void LoggerOutputDevice::Write(std::string &&text)
{
	_logger.AddLog(text);
}
}
