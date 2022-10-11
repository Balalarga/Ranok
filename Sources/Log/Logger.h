#pragma once

#include <string>
#include <vector>
#include <memory>


namespace Ranok
{
class ILogOutputDevice;

enum class LogLevel
{
    Verbose, Log, Warning, Error
};

class Logger
{
public:
    static void Print(LogLevel level, std::string&& text);
    
    /// Careful, it creates unique_ptr for device
    static void AddOutputDevice(ILogOutputDevice* device);

    static void Log(std::string&& text);
    static void Warning(std::string&& text);
    static void Error(std::string&& text);
    
    static void Log(const std::string& text);
    static void Warning(const std::string& text);
    static void Error(const std::string& text);

private:
    static std::vector<std::unique_ptr<ILogOutputDevice>> devices;
};
}

