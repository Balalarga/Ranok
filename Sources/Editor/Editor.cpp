#include "Editor.h"
#include "imgui.h"
#include "Modules/EditorModule.h"
#include "Localization/LocalizationSystem.h"
#include "Log/Logger.h"
#include "Settings/ISettings.h"
#include "Settings/SettingsManager.h"
#include "Utils/FileUtils.h"
#include "Utils/WindowsUtils.h"
#include "Utils/Archives/Json/JsonArchive.h"

namespace Ranok
{
DEFINE_LOCTEXT(FileMenu, "File")
DEFINE_LOCTEXT(ModulesMenu, "Modules")
DEFINE_LOCTEXT(SettinsMenu, "Settings")


class EditorSettings: public ISettings
{
public:
	EditorSettings(): ISettings("Editor/EditorSettings")
	{}
	
	void Serialize(JsonArchive& archive) override
	{
		archive.Serialize("DefaultLayoutIniPath", defaultLayoutIni);
	}

	std::string defaultLayoutIni;
};
std::shared_ptr<EditorSettings> editorSettings;


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
	editorSettings = SettingsManager::Instance().CreateSettings<EditorSettings>();
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
		if (ImGui::BeginMenu(LOCTEXT(FileMenu)))
		{
			if (ImGui::MenuItem("Open"))
			{
				std::string filepathStr = OpenFileDialog();
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
		
		if (ImGui::BeginMenu(LOCTEXT(SettinsMenu)))
		{
			if (ImGui::MenuItem("Save as default layout"))
				ImGui::SaveIniSettingsToDisk(editorSettings->defaultLayoutIni.c_str());
			ImGui::EndMenu();
		}
		
		if (ImGui::BeginMenu(LOCTEXT(ModulesMenu)))
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
	if (Files::IsFileExists(editorSettings->defaultLayoutIni))
	{
		Logger::Error(fmt::format("Default layout loaded from {}", editorSettings->defaultLayoutIni));
		ImGui::LoadIniSettingsFromDisk(editorSettings->defaultLayoutIni.c_str());
	}
	else
	{
		Logger::Error(fmt::format("Couldn't load default layout from {}", editorSettings->defaultLayoutIni));
	}
}
}
 