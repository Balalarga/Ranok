#include "ModelingModule.h"
#include "Localization/LocalizationSystem.h"

namespace Ranok
{
DEFINELOCALETEXT(ModuleName, "Modeling")

ModelingModule::ModelingModule():
	IEditorModule(GETLOCALETEXTSTR(ModuleName))
{
}

void ModelingModule::RenderWindowContent()
{
	ImGui::BeginChild("Splitter test");

	static float w = 200.0f;
	static float h = 300.0f;
	float trueH = h >= 0 ? h : 1;
	float trueW = w >= 0 ? w : 1;
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0,0));
	ImGui::BeginChild("child1", ImVec2(trueW, trueH), true);
	ImGui::EndChild();
	ImGui::SameLine();
	ImGui::InvisibleButton("vsplitter", ImVec2(8.0f, trueH));
	bool bIsActive = ImGui::IsItemActive();
	bool bIsHovered = ImGui::IsItemHovered();
	if (bIsActive)
		w += ImGui::GetIO().MouseDelta.x;
	if (bIsActive || bIsHovered)
		ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);
	ImGui::SameLine();
	ImGui::BeginChild("child2", ImVec2(0, trueH), true);
	ImGui::EndChild();
	ImGui::InvisibleButton("hsplitter", ImVec2(-1, 8.0f));
	bIsActive = ImGui::IsItemActive();
	bIsHovered = ImGui::IsItemHovered();
	if (ImGui::IsItemActive())
		h += ImGui::GetIO().MouseDelta.y;
	if (bIsActive || bIsHovered)
		ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeNS);
	ImGui::BeginChild("child3", ImVec2(0, 0), true);
	ImGui::EndChild();
	ImGui::PopStyleVar();

	ImGui::EndChild();
}
}
