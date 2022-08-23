#pragma once
#include <variant>
#include <string>

#include <imgui.h>

#include "FileUtils.h"

namespace Ranok::StyleLoader
{
std::variant<ImGuiStyle, std::string> LoadStyle(const std::string& filepath)
{
	auto fileData = Ranok::Files::ReadAsset(filepath);
	if (!fileData.has_value())
		return "Couldn't read file " + filepath;
	
	return "Unknown error";
}
}
