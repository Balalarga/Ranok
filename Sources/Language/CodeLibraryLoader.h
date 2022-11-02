#pragma once
#include <optional>

#include "ActionNode.h"
#include "Parser.h"

#include "Utils/FileUtils.h"

namespace Ranok
{
inline std::optional<ActionNodeFactory> LoadLibrary(const std::string& path)
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

inline std::map<std::string, ActionNodeFactory> LoadDefaultLibraries()
{
	namespace fs = std::filesystem;
	std::map<std::string, ActionNodeFactory> libs;
	for (const fs::directory_entry& entry : fs::recursive_directory_iterator(SystemSettings::defaultLibrariesDir))
	{
		if (!entry.is_directory() && entry.path().extension().compare(RANOK_CODE_EXTENSION) == 0)
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
