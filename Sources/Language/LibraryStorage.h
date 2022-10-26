#pragma once
#include <filesystem>
#include <map>
#include <string>

namespace Ranok
{
class ActionNodeFactory;

class LibraryStorage
{
public:
	static LibraryStorage& Instance();
	static void LoadDefaults();
	static const std::map<std::string, ActionNodeFactory>& GetLibs() { return Instance()._libs; } 

	bool Add(const std::filesystem::path& filepath);
	
private:
	LibraryStorage() = default;

	std::map<std::string, ActionNodeFactory> _libs;
};
}
