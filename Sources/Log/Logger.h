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
    static void Print(LogLevel level, std::string&& text);
    
    /// Careful, it creates unique_ptr for device
    static void AddOutputDevice(IOutputDevice* device);

    static void Log(std::string&& text);
    static void Warning(std::string&& text);
    static void Error(std::string&& text);

private:
    static std::vector<std::unique_ptr<IOutputDevice>> devices;
};
}

