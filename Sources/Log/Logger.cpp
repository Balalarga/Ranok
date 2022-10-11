#include "Logger.h"
#include "OutputDevice.h"
#include "fmt/format.h"

namespace Ranok
{
std::vector<std::unique_ptr<ILogOutputDevice>> Logger::devices = {};

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
		if (static_cast<int>(level) >= static_cast<int>(device->GetMinLogLevel()))
			device->Write(level, std::forward<std::string>(leveledText));
	}
}

void Logger::AddOutputDevice(ILogOutputDevice* device)
{
	devices.push_back(std::unique_ptr<ILogOutputDevice>(device));
}

void Logger::Log(std::string&& text)
{
	Print(LogLevel::Log, fmt::format("{}", text));
}

void Logger::Warning(std::string&& text)
{
	Print(LogLevel::Warning, fmt::format("{}", text));
}

void Logger::Error(std::string&& text)
{
	Print(LogLevel::Error, fmt::format("{}", text));
}

void Logger::Log(const std::string& text)
{
	Print(LogLevel::Log, fmt::format("{}", text));
}

void Logger::Warning(const std::string& text)
{
	Print(LogLevel::Warning, fmt::format("{}", text));
}

void Logger::Error(const std::string& text)
{
	Print(LogLevel::Error, fmt::format("{}", text));
}
}
