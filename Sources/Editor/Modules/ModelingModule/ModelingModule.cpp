#include "ModelingModule.h"
#include "Localization/LocalizationSystem.h"
#include "Utils/FileUtils.h"

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
	static float w = ImGui::GetWindowContentRegionMax().x / 3.f;
	float trueH = ImGui::GetWindowContentRegionMax().y;
	float trueW = w >= 0 ? w : 1;
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
	ImGui::BeginChild("child1", ImVec2(trueW, trueH), true);
	if (!_textEditorTabs.empty())
	{
		int idToClose = -1;
		ImGui::BeginTabBar("##textEditorTabs");
		for (size_t i = 0; i < _textEditorTabs.size(); ++i)
		{
			bool bOpen = true;
			if (ImGui::BeginTabItem(_textEditorTabs[i].GetFilename().c_str(), &bOpen))
			{
				_textEditorTabs[i].Render();
				ImGui::EndTabItem();
			}
			if (!bOpen)
			{
				idToClose = static_cast<int>(i);
				ImGui::OpenPopup("Close rcode");
				break;
			}
		}
		ImGui::EndTabBar();
		if (ImGui::BeginPopupModal("Close rcode"))
		{
			Logger::Log("Popup");
			if (ImGui::Button("Yes"))
				ImGui::CloseCurrentPopup();
			ImGui::SameLine();
			if (ImGui::Button("No"))
				ImGui::CloseCurrentPopup();
			ImGui::EndPopup();
		}
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
	ImGui::PopStyleVar(2);
	ImGui::EndChild();
}

void ModelingModule::PostRender()
{
}

bool ModelingModule::TryOpenFile(const std::string& filepath)
{
	if (!filepath.ends_with(".rcode"))
		return false;
	
	std::optional<std::string> data = Files::ReadFile(filepath);
	if (!data.has_value())
		return false;
	
	TextEditor& lastItem = _textEditorTabs.emplace_back();
	lastItem.SetText(data.value());
	size_t nameStart = filepath.find_last_of("\\")+1;
	lastItem.SetFilename(filepath.substr(nameStart));
	lastItem.SetFilepath(filepath);
	return true;
}
}
