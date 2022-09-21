#pragma once

#include <iostream>
#include <vector>
#include <memory>
#include "fmt/format.h"
#include "OutputDevice.h"


namespace Ranok
{
enum class LogLevel
{
    Log, Warning, Error
};

class Logger
{
public:
    /// Careful, it creates unique_ptr for device
    static void AddOutputDevice(IOutputDevice* device)
    {
        devices.push_back(std::unique_ptr<IOutputDevice>(device));
    }

    static void Print(LogLevel level, std::string&& text)
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

    static void Log(std::string&& text)
    {
        Print(LogLevel::Log, fmt::format("{}\n", text));
    }

    static void Warning(std::string&& text)
    {
        Print(LogLevel::Warning, fmt::format("{}\n", text));
    }

    static void Error(std::string&& text)
    {
        Print(LogLevel::Error, fmt::format("{}\n", text));
    }

private:
    static std::vector<std::unique_ptr<IOutputDevice>> devices;
};
}

