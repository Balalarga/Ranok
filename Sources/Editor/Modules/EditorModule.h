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
		}});
	}
	
	virtual void RenderWindowContent() = 0;
	
	std::string moduleName;
	bool bWorks = false;
};
}
