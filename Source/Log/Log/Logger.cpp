#include "Logger.h"
#include "LogOutputDevice.h"

namespace Ranok
{
Logger& Logger::Instance()
{
	static Logger logger;
	return logger;
}

void Logger::Print(LogLevel level, std::string&& text)
{
	std::string leveledText;
	switch(level)
	{
	case LogLevel::Log:
		leveledText = "[Log]     ";
		break;
	case LogLevel::Warning:
		leveledText = "[Warning] ";
		break;
	case LogLevel::Error:
		leveledText = "[Error]   ";
		break;
	case LogLevel::Verbose:
		leveledText = "[Verbose] ";
		break;
	}
	leveledText += text;

	for (const std::unique_ptr<ILogOutputDevice>& device : devices)
	{
		if (static_cast<uint8_t>(level) >= static_cast<uint8_t>(device->GetMinLogLevel()))
			device->Write(level, std::forward<std::string>(leveledText));
	}
}

void Logger::AddOutputDevice(ILogOutputDevice* device)
{
	Instance().devices.push_back(std::unique_ptr<ILogOutputDevice>(device));
}

void Logger::Log(std::string&& text)
{
	Instance().Print(LogLevel::Log, text.c_str());
}

void Logger::Warning(std::string&& text)
{
	Instance().Print(LogLevel::Warning, text.c_str());
}

void Logger::Error(std::string&& text)
{
	Instance().Print(LogLevel::Error, text.c_str());
}

void Logger::Verbose(std::string&& text)
{
	Instance().Print(LogLevel::Verbose, text.c_str());
}

void Logger::Log(const std::string& text)
{
	Instance().Print(LogLevel::Log, text.c_str());
}

void Logger::Warning(const std::string& text)
{
	Instance().Print(LogLevel::Warning, text.c_str());
}

void Logger::Error(const std::string& text)
{
	Instance().Print(LogLevel::Error, text.c_str());
}

void Logger::Verbose(const std::string& text)
{
	Instance().Print(LogLevel::Verbose, text.c_str());
}

Logger::Logger()
{
	devices.push_back(std::make_unique<CmdOutput>());
}
}
