#include "ModelingModule.h"
#include "Localization/LocalizationSystem.h"

namespace Ranok
{
DEFINELOCALETEXT(ModuleName, "Modeling")

ModelingModule::ModelingModule():
	IEditorModule(GETLOCALETEXTSTR(ModuleName)),
	_viewport({800, 600})
{
	_viewport.Create();
}

void ModelingModule::RenderWindowContent()
{
	ImGui::BeginChild("mainContainer");
	static float w = 200.0f;
	float trueH = ImGui::GetWindowContentRegionMax().y;
	float trueW = w >= 0 ? w : 1;
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
	ImGui::BeginChild("child1", ImVec2(trueW, trueH), true);
	if (_textEditorTabs.size() > 0)
	{
		ImGui::BeginTabBar("##textEditorTabs");
		for (auto& tab: _textEditorTabs)
		{
			if (ImGui::BeginTabItem(""))
			{
				tab.Render("TextEditor");
			}
			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();
	}
	
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
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::BeginChild("child2", ImVec2(0, trueH), true);
	ImGui::Image((void*)(intptr_t)_viewport.GetTextureId(),
				 ImGui::GetWindowSize(),
				 ImVec2(0, 1),
				 ImVec2(1, 0));
	ImGui::EndChild();
	ImGui::PopStyleVar();
	ImGui::PopStyleVar();
	ImGui::EndChild();
}
}
