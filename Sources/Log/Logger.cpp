#include "Logger.h"

namespace Ranok
{
std::vector<std::unique_ptr<IOutputDevice>> Logger::devices = {};

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
	}
	leveledText += text;

	for (auto& device: devices)
		device->Write(std::forward<std::string>(leveledText));
}

void Logger::AddOutputDevice(IOutputDevice* device)
{
	devices.push_back(std::unique_ptr<IOutputDevice>(device));
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
}
