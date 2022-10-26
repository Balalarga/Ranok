#include "LibraryStorage.h"

#include "CodeLibraryLoader.h"

namespace Ranok
{

LibraryStorage& LibraryStorage::Instance()
{
	static LibraryStorage sSelf;
	return sSelf;
}

void LibraryStorage::LoadDefaults()
{
	std::map<std::string, ActionNodeFactory>& libs = Instance()._libs;
	libs = LoadDefaultLibraries();
}

bool LibraryStorage::Add(const std::filesystem::path& filepath)
{
	std::map<std::string, ActionNodeFactory>& libs = Instance()._libs;
	std::string filename = filepath.filename().generic_string();
	if (libs.contains(filename))
		return true;

	std::optional<ActionNodeFactory> lib = LoadLibrary(filepath.string());
	if (!lib.has_value())
		return false;

	libs[filename] = lib.value();
	return true;
}
}
