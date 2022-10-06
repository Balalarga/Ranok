#include "ModelingModule.h"
#include "Localization/LocalizationSystem.h"
#include "Utils/FileUtils.h"
#include "Utils/WindowsUtils.h"

namespace Ranok
{
DEFINE_LOCTEXT(ModuleName, "Modeling")
DEFINE_LOCTEXT(CloseTabText, "Do you want to save code before closing?")
DEFINE_LOCTEXT(CloseTabSaveText, "Save")
DEFINE_LOCTEXT(CloseTabCloseText, "Close")
DEFINE_LOCTEXT(CloseTabCancelText, "Cancel")

ModelingModule::ModelingModule():
	IEditorModule(LOCTEXTSTR(ModuleName)),
	_viewport({800, 600})
{
	_viewport.Create();
}

void ModelingModule::RenderWindowContent()
{
	ImGui::BeginChild("mainContainer");
	static float w = ImGui::GetWindowContentRegionMax().x / 3.f;
	const float trueH = ImGui::GetWindowContentRegionMax().y;
	const float trueW = w >= 0 ? w : 1;
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
	ImGui::BeginChild("child1", ImVec2(trueW, trueH), true);
	if (!_textEditorTabs.empty())
	{
		static int idToClose = -1;
		ImGui::BeginTabBar("##textEditorTabs");
		for (size_t i = 0; i < _textEditorTabs.size(); ++i)
		{
			bool bOpen = true;
			if (ImGui::BeginTabItem(_textEditorTabs[i].filename.c_str(), &bOpen))
			{
				_textEditorTabs[i].editor.Render(_textEditorTabs[i].filename.c_str());
				ImGui::EndTabItem();
			}
			if (!bOpen)
			{
				idToClose = static_cast<int>(i);
				ImGui::OpenPopup("Close code");
				break;
			}
		}
		
		if (ImGui::BeginPopupModal("Close code"))
		{
			ImGui::Text(LOCTEXT(CloseTabText));
			ImGui::Text("\n");
			ImGui::Separator();

			if (ImGui::Button(LOCTEXT(CloseTabSaveText)))
			{
				SaveFileDialog("");
				ImGui::CloseCurrentPopup();
				_textEditorTabs.erase(_textEditorTabs.begin() + idToClose);
			}
			
			ImGui::SameLine();
			if (ImGui::Button(LOCTEXT(CloseTabCloseText)))
			{
				ImGui::CloseCurrentPopup();
				_textEditorTabs.erase(_textEditorTabs.begin() + idToClose);
			}
			
			ImGui::SameLine();
			if (ImGui::Button(LOCTEXT(CloseTabCancelText)))
				ImGui::CloseCurrentPopup();
			
			ImGui::EndPopup();
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
	
	TextEditorInfo& lastItem = _textEditorTabs.emplace_back();
	lastItem.editor.SetText(data.value());
	size_t nameStart = filepath.find_last_of("\\")+1;
	lastItem.filename = filepath.substr(nameStart);
	lastItem.filepath = filepath;
	return true;
}
}
