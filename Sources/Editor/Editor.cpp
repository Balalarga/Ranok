#include "Editor.h"

#include "imgui.h"
#include "ImGuiFileDialog.h"
#include "Modules/EditorModule.h"
#include "Localization/LocalizationSystem.h"
#include "Log/Logger.h"
#include "Utils/FileUtils.h"
#include "Utils/WindowsUtils.h"

namespace Ranok
{
DEFINELOCALETEXT(FileMenu, "File")
DEFINELOCALETEXT(ModulesMenu, "Modules")
DEFINELOCALETEXT(SettinsMenu, "Settings")

ModuleSystem<IEditorModule> Editor::EditorSystem;

Editor& Editor::Instance()
{
    static Editor editor;
    return editor;
}

Editor::~Editor()
{

}

Editor::Editor()
{
    AddGuiLayer(GuiLayer([this] { GuiRender(); }));
	TryLoadDefaultLayout();
}

void Editor::GuiRender()
{
	ImGui::PushStyleColor(ImGuiCol_DockingEmptyBg, ImVec4(0.02f, 0.02f, 0.02f, 1.f));
	
	constexpr ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar |
		ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
	
	const ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(viewport->WorkPos);
	ImGui::SetNextWindowSize(viewport->WorkSize);
	ImGui::SetNextWindowViewport(viewport->ID);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::Begin("MainEditorWindow", nullptr, window_flags);
	ImGui::PopStyleVar(3);
	
	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu(GETLOCALETEXT(FileMenu)))
		{
			if (ImGui::MenuItem("Open"))
			{
				std::string filepathStr = OpenFileDialog("*.*");
				if (!filepathStr.empty())
				{
					Logger::Log(std::forward<std::string>(filepathStr));
					bool bProcessed = false;
					EditorSystem.EnumerateModules([&bProcessed, &filepathStr](IEditorModule* module)
					{
						if (!bProcessed)
							bProcessed = module->bWorks && module->TryOpenFile(filepathStr);
					});
					if (!bProcessed)
						Logger::Warning(fmt::format("No one module can open file: {}", filepathStr));
				}
			}
			ImGui::EndMenu();
		}
		
		if (ImGui::BeginMenu(GETLOCALETEXT(SettinsMenu)))
		{
			if (ImGui::MenuItem("Save as default layout"))
				ImGui::SaveIniSettingsToDisk(Files::GetDefaultLayoutConfigPath().c_str());
			ImGui::EndMenu();
		}
		
		if (ImGui::BeginMenu(GETLOCALETEXT(ModulesMenu)))
		{
			EditorSystem.EnumerateModules([](IEditorModule* module)
			{
				ImGui::MenuItem(module->moduleName.c_str(), nullptr, &module->bWorks);
			});
			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}
	
	static ImGuiID dockspaceId = ImGui::GetID("MainDockSpace");
	ImGui::DockSpace(dockspaceId, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);
	
	ImGui::End();
	ImGui::PopStyleColor();
}

void Editor::TryLoadDefaultLayout()
{
	std::string layoutPath = Files::GetDefaultLayoutConfigPath();
	if (Files::IsFileExists(layoutPath))
	{
		Logger::Error(fmt::format("Default layout loaded from {}", layoutPath));
		ImGui::LoadIniSettingsFromDisk(layoutPath.c_str());
	}
	else
	{
		Logger::Error(fmt::format("Couldn't load default layout from {}", layoutPath));
	}
}
}
 