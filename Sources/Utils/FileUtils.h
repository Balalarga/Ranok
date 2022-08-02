#pragma once
#include <sstream>
#include <fstream>
#include <optional>
#include <string>

namespace Ranok::Files
{
inline std::optional<std::string> ReadFile(const std::string& path)
{	
	std::ifstream file(path);
	if (!file)
		return {};
	
	std::stringstream stream;
	stream << file.rdbuf();
	file.close();
	return stream.str();
}
}
