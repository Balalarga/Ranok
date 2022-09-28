#pragma once
#include <sstream>
#include <fstream>
#include <optional>
#include <string>
#include <fmt/format.h>
#include "Language/ActionNode.h"
#include "Language/Parser.h"

namespace Ranok::Files
{
inline std::string GetAssetPath(const std::string& relativePath)
{
	return fmt::format("{}/{}", RESOURCE_DIR, relativePath);
}

inline std::string GetDefaultLayoutConfigPath()
{
	return GetAssetPath("Config/DefaultLayout.ini");
}

inline bool IsFileExists(const std::string& path)
{
	std::ifstream file(path);
	bool bExists = !!file;
	file.close();
	return bExists;
}

inline bool IsAssetExists(const std::string& relativePath)
{
	return IsFileExists(GetAssetPath(relativePath));
}

inline std::optional<std::string> ReadFile(const std::string& path)
{
	std::ifstream file(path);
	if (!file)
	{
		std::cerr << path << std::endl;
		return {};
	}
	
	std::stringstream stream;
	stream << file.rdbuf();
	file.close();
	return stream.str();
}

inline std::optional<std::string> ReadAsset(const std::string& relativePath)
{
	return ReadFile(GetAssetPath(relativePath));
}

inline std::optional<ActionNodeFactory> LoadLibrary(const std::string& path)
{
	Parser parser;
	std::optional<std::string> code = ReadFile(path);
	if (!code.has_value())
		return {};

	ActionTree tree = parser.Parse(Lexer(code.value()));
	
	if (parser.HasErrors())
	{
		std::cerr << "----------------------------------------\n";
		std::cerr << path << std::endl;
		for(auto& err: parser.Errors())
			std::cerr << err << std::endl;
		std::cerr << "----------------------------------------\n";
	}
	return tree.GlobalFactory();
}

inline std::optional<ActionNodeFactory> LoadAssetLibrary(const std::string& relativePath)
{
	return LoadLibrary(fmt::format("{}/{}", SDL_LIBRARY_DIR, relativePath));
}
}
