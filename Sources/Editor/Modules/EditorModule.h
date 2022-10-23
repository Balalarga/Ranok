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

	// Use for On OpenFile execution for yours module
	virtual bool TryOpenFile(const std::string& filepath) { return false; }
	
	std::string moduleName;
	bool bWorks = false;
	
private:
	ImGuiWindowFlags flags;
	bool bNotClosing = false;
};
}
