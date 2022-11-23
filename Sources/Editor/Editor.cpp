#include "Editor.h"
#include "imgui.h"
#include "Modules/EditorModule.h"
#include "Localization/LocalizationSystem.h"
#include "Log/Logger.h"
#include "Config/IConfig.h"
#include "Config/ConfigManager.h"
#include "Utils/FileUtils.h"
#include "Utils/Archives/Json/JsonArchive.h"

namespace Ranok
{
DEFINE_LOCTEXT(EditorModulesMenu, "Modules")
DEFINE_LOCTEXT(EditorSettinsMenu, "Settings")
DEFINE_LOCTEXT(EditorPreferences, "Preferences")


class EditorConfigs: public IConfig
{
public:
	EditorConfigs(): IConfig("Editor/EditorConfigs", true)
	{
	}
	
	void Serialize(JsonArchive& archive) override
	{
		archive.Serialize("DefaultLayoutIniPath", defaultLayoutIni);
		archive.Serialize("WindowSize", windowSize);
	}
	bool operator!=(const IConfig& oth) override
	{
		const auto casted = dynamic_cast<const EditorConfigs*>(&oth);
		return casted && 
			casted->windowSize != windowSize &&
			casted->defaultLayoutIni != defaultLayoutIni;
	}

	std::string defaultLayoutIni;
	glm::uvec2 windowSize{1280, 720};
};
std::shared_ptr<EditorConfigs> editorConfigs = ConfigManager::Instance().CreateConfigs<EditorConfigs>();

ModuleSystem<IEditorModule> Editor::EditorSystem;

Editor& Editor::Instance()
{
    static Editor editor;
    return editor;
}

Editor::Editor()
{
    AddGuiLayer(GuiLayer([this] { GuiRender(); }));
	TryLoadDefaultLayout();
	Resize(editorConfigs->windowSize);
	SDL_SetWindowPosition(GetSdlWindow(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
}

void Editor::GuiRender()
{
	static bool showStyleEditor = false;
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
		if (ImGui::BeginMenu(LOCTEXT(EditorSettinsMenu)))
		{
			if (ImGui::MenuItem("Save as default layout"))
				ImGui::SaveIniSettingsToDisk(Files::GetAssetPath(editorConfigs->defaultLayoutIni).c_str());
			if (ImGui::MenuItem("EditorPreferences"))
				ImGui::OpenPopup(LOCTEXT(EditorPreferences));
			ImGui::EndMenu();
		}
#ifdef DEBUG_MODE
		if (ImGui::BeginMenu("Editor Settings"))
		{
			if (ImGui::MenuItem("Style setup"))
				showStyleEditor = true;
			ImGui::EndMenu();
		}
#endif
		if (ImGui::BeginMenu(LOCTEXT(EditorModulesMenu)))
		{
			EditorSystem.EnumerateModules([](IEditorModule* module)
			{
				ImGui::MenuItem(module->moduleName.c_str(), nullptr, &module->bWorks);
			});
			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}
	
	if (showStyleEditor)
	{
		ImGui::Begin("StyleEditor", &showStyleEditor);
		ImGui::ShowStyleEditor();
		ImGui::End();
	}

	ImGui::ShowDemoWindow();
	if (ImGui::BeginPopupModal(LOCTEXT(EditorPreferences)))
	{
		ShowPreferencesWindow();
		ImGui::EndPopup();
	}
	
	static ImGuiID dockspaceId = ImGui::GetID("MainDockSpace");
	ImGui::DockSpace(dockspaceId, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);
	
	ImGui::End();
	ImGui::PopStyleColor();
}

void Editor::TryLoadDefaultLayout() const
{
	std::string path = Files::GetAssetPath(editorConfigs->defaultLayoutIni);
	if (Files::IsFileExists(path))
	{
		ImGui::LoadIniSettingsFromDisk(path.c_str());
		Logger::Verbose(fmt::format("Default layout loaded from {}", path));
	}
	else
	{
		Logger::Error(fmt::format("Couldn't load default layout from {}", path));
	}
}

void Editor::OnResize(glm::uvec2 size)
{
	OpenglWindow::OnResize(size);
	editorConfigs->windowSize = size;
}

void Editor::ShowPreferencesWindow()
{
	std::map<std::string, std::shared_ptr<IConfig>> configs = ConfigManager::Instance().GetConfigs();
	static int selectedItem = -1;
	ImGui::BeginChild("#settingsNames", ImVec2(ImGui::GetWindowWidth()/3, 0));
	ImGui::BeginListBox("#names");
	int counter = 0;
	for (const auto& [name, config] : configs)
	{
		const bool is_selected = selectedItem == counter;
		// if (config->HaveGui() && ImGui::Selectable(name.c_str(), is_selected))
		if (ImGui::Selectable(name.c_str(), is_selected))
			selectedItem = counter;

		if (is_selected)
			ImGui::SetItemDefaultFocus();
		
		++counter;
	}
	ImGui::EndListBox();
	ImGui::EndChild();

	ImGui::BeginChild("#settingContent");
	ImGui::EndChild();
}

}
