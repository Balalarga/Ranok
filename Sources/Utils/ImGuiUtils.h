#pragma once
#include <map>

#include "imgui.h"

namespace Ranok::ImGuiUtils
{
struct ScopePushVar
{
	ScopePushVar(const std::map<int, float>& vars):
		count(static_cast<int>(vars.size()))
	{
		for (auto& kv: vars)
			ImGui::PushStyleVar(kv.first, kv.second);
	}
	
	ScopePushVar(const std::map<int, ImVec2>& vars):
		count(static_cast<int>(vars.size()))
	{
		for (auto& kv: vars)
			ImGui::PushStyleVar(kv.first, kv.second);
	}
	
	~ScopePushVar()
	{
		ImGui::PopStyleVar(count);
	}
	int count;
};
}
