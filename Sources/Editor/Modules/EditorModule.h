#pragma once
#include <string>

#include "imgui.h"
#include "Editor/Editor.h"
#include "Utils/ModuleSystem.h"

namespace Ranok
{
class IEditorModule: public IModule
{
public:
	IEditorModule(const std::string& inModuleName, ImGuiWindowFlags inFlags = ImGuiWindowFlags_None):
		moduleName(inModuleName),
		flags(inFlags)
	{
		ImGui::GetStyle().WindowMenuButtonPosition = ImGuiDir_None;

		Editor::Instance().AddGuiLayer({[this]
		{
			if (!bWorks)
				return;

			if (bNotClosing)
			{
				if (ImGui::Begin(moduleName.c_str(), nullptr, flags))
					RenderWindowContent();
			}
			else
			{
				if (ImGui::Begin(moduleName.c_str(), &bWorks, flags))
					RenderWindowContent();
			}
			
			ImGui::End();
			
			PostRender();
		}});
	}
	
	virtual void PostRender() {}
	virtual void RenderWindowContent() = 0;
	void SetNoClosing(bool bClose) { bNotClosing = bClose; }
	
	std::string moduleName;
	bool bWorks = false;
	
private:
	ImGuiWindowFlags flags;
	bool bNotClosing = false;
};
}
