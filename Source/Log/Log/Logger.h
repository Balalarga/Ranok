#pragma once

#include <string>
#include <vector>
#include <memory>
#include "fmt/format.h"

namespace Ranok
{
class ILogOutputDevice;

enum class LogLevel: uint8_t
{
    Verbose = 0,
    Log,
    Warning,
    Error
};

class Logger
{
public:
    static Logger& Instance();
    void Print(LogLevel level, std::string&& text);
    
    /// Careful, it creates unique_ptr for device
    static void AddOutputDevice(ILogOutputDevice* device);

    static void Log(std::string&& text);
    static void Warning(std::string&& text);
    static void Error(std::string&& text);
    static void Verbose(std::string&& text);
    
    static void Log(const std::string& text);
    static void Warning(const std::string& text);
    static void Error(const std::string& text);
    static void Verbose(const std::string& text);

private:
    Logger();
    
    std::vector<std::unique_ptr<ILogOutputDevice>> devices;
};
}

