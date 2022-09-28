#pragma once
#include <string>

#include "imgui.h"
#include "imgui_internal.h"
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
		using namespace std::placeholders; // for _1, _2 etc.
		//https://github.com/ocornut/imgui/issues/2564
		ImGuiSettingsHandler ini_handler;
		ini_handler.TypeName = moduleName.c_str();
		ini_handler.TypeHash = ImHashStr(moduleName.c_str());
		ini_handler.ReadOpenFn = this->ReadOpenConfig;// WindowSettingsHandler_ReadOpen
		ini_handler.ReadLineFn = ;// WindowSettingsHandler_ReadLine
		ini_handler.WriteAllFn = ;// WindowSettingsHandler_WriteAll
		GImGui->SettingsHandlers.push_back(ini_handler);
	}
	virtual void* ReadOpenConfig(ImGuiContext* ctx, ImGuiSettingsHandler* handler, const char* name) { return nullptr; }
	virtual void ReadLineConfig(ImGuiContext* ctx, ImGuiSettingsHandler* handler, void* entry, const char* line) {}
	virtual void WriteAllConfig(ImGuiContext* ctx, ImGuiSettingsHandler* handler, ImGuiTextBuffer* out_buf) {}
	
	virtual void RenderWindowContent() = 0;
	
	std::string moduleName;
	bool bWorks = false;
};
}
