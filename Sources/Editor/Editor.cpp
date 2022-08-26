#include "Editor.h"

#include <imgui_internal.h>

#include "imgui.h"
#include "Utils/ImGuiUtils.h"

namespace Ranok
{
Editor::Editor()
{
	ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	AddGuiLayer(GuiLayer([this] { GuiRender(); }));
}

void Editor::GuiRender()
{
	ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::DockSpaceOverViewport(viewport);
	
	ImGui::Begin("MainEditorWindow", nullptr);
	ImGui::End();
}
}
