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
	IEditorModule(const std::string& inModuleName):
		moduleName(inModuleName)
	{
		Editor::Instance().AddGuiLayer({[this]
		{
			if (!bWorks)
				return;
			
			if (ImGui::Begin(moduleName.c_str(), &bWorks))
				RenderWindowContent();
			
			ImGui::End();
			
			PostRender();
		}});
	}
	
	virtual void PostRender() {}
	virtual void RenderWindowContent() = 0;

	// Use for On OpenFile execution for yours module
	virtual bool TryOpenFile(const std::string& filepath) { return false; }
	
	std::string moduleName;
	bool bWorks = false;
};
}
