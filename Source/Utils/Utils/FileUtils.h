#pragma once
#include <filesystem>
#include <sstream>
#include <fstream>
#include <optional>
#include <string>
#include <fmt/format.h>

#include "Log/Logger.h"

namespace Ranok::Files
{
inline std::string GetAssetPath(const std::string& relativePath)
{
	
#if defined(_DEBUG) || defined(DEBUG)
	return fmt::format(PROJECT_SOURCE_DIR"{}/{}", "\\Assets", relativePath);
#else
	return fmt::format("{}/{}", ".\\Assets", relativePath);
#endif
}

inline bool IsFileExists(const std::string& path)
{
	std::ifstream file(path);
	const bool bExists = !!file;
	file.close();
	return bExists;
}

inline bool IsAssetExists(const std::string& relativePath)
{
	return IsFileExists(GetAssetPath(relativePath));
}

/// Create file and all parent directories for path
inline void CreateFile(const std::string& filepath)
{
	const std::filesystem::path path(filepath);
	create_directories(path.parent_path());
	
	std::ofstream file(filepath);
	file.close();
}

inline void CreateAsset(const std::string& relativePath)
{
	CreateFile(GetAssetPath(relativePath));
}

inline std::optional<std::string> ReadFile(const std::string& path, bool bBinary = false)
{
	std::ifstream file(path, bBinary ? std::ios_base::binary : std::ios_base::_Default_open_prot);
	if (!file)
	{
		Logger::Error(fmt::format("Couldn't open {}  for reading", path));
		return std::nullopt;
	}
	
	std::stringstream stream;
	stream << file.rdbuf();
	file.close();
	return stream.str();
}

inline bool WriteToFile(const std::string& path, const std::string& data)
{
	std::ofstream file(path);
	if (!file)
	{
		Logger::Error(fmt::format("Couldn't open {} for writing", path));
		return false;
	}
	file << data;
	file.close();
	return true;
}

inline std::optional<std::string> ReadAsset(const std::string& relativePath)
{
	return ReadFile(GetAssetPath(relativePath));
}
}
