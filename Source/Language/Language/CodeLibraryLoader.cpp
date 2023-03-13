#include "CodeLibraryLoader.h"

namespace Ranok
{
std::optional<ActionNodeFactory> LoadLibrary(const std::string& path)
{
	Parser parser;
	std::optional<std::string> code = Files::ReadFile(path);
	if (!code.has_value())
		return std::nullopt;
	
	ActionTree tree = parser.Parse(Lexer(code.value()));
	
	if (parser.HasErrors())
	{
		std::stringstream errorText;
		errorText << "----------------------------------------\n";
		errorText << path << std::endl;
		for(const auto& err : parser.Errors())
			errorText << err.text << std::endl;
		errorText << "----------------------------------------\n";
		Logger::Error(errorText.str());
	}
	return tree.GlobalFactory();
}

std::map<std::string, ActionNodeFactory> LoadDefaultLibraries()
{
	static std::string defaultLibrariesDir = 
#if defined(_DEBUG) || defined(DEBUG)
	PROJECT_SOURCE_DIR"/Assets/CodeLibrary";
#else
	"Assets/CodeLibrary";
#endif
	namespace fs = std::filesystem;
	std::map<std::string, ActionNodeFactory> libs;
	for (const fs::directory_entry& entry : fs::recursive_directory_iterator(defaultLibrariesDir))
	{
		if (!entry.is_directory() && entry.path().extension().compare(".rcode") == 0)
		{
			std::optional<ActionNodeFactory> lib = LoadLibrary(entry.path().generic_string());
			if (lib.has_value())
				libs[entry.path().filename().generic_string()] = lib.value();
			else
				Logger::Error(fmt::format("Couldn't load code library from ", entry.path().generic_string()));
		}
	}
	return libs;
}
}