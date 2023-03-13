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
DEFINE_LOCTEXT(EditorPreferences, "Settings")


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
        assert(casted);
		return
			casted->windowSize != windowSize ||
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
	ImGui::GetIO().WantSaveIniSettings = false;
    AddGuiLayer(GuiLayer([this] { GuiRender(); }));
	TryLoadDefaultLayout();
	Resize(editorConfigs->windowSize);
	SDL_SetWindowPosition(GetSdlWindow(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);
}

void Editor::ShowPreferencesWindow()
{
	std::vector<std::shared_ptr<IConfig>> configs = ConfigManager::Instance().GetConfigs();
	static auto namesSize = ImVec2(ImGui::GetWindowWidth()/3, 0);
	static int item_current_idx = -1;
	if (ImGui::BeginChild("#settingsNames", namesSize, true))
	{
		if (ImGui::BeginListBox("##empty", namesSize))
		{
			for (int n = 0; n < configs.size(); ++n)
			{
				if (!configs[n]->HaveGui())
					continue;
			
				const bool is_selected = item_current_idx == n;
				if (ImGui::Selectable(configs[n]->GetName().c_str(), is_selected))
					item_current_idx = n;

				if (is_selected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndListBox();
		}
	}
	ImGui::EndChild();
	ImGui::SameLine();
	
	ImGui::InvisibleButton("SettingsVsplitter", ImVec2(5.0f, ImGui::GetWindowContentRegionMax().y));
	const bool bIsActive = ImGui::IsItemActive();
	const bool bIsHovered = ImGui::IsItemHovered();
	if (bIsActive)
		namesSize.x += ImGui::GetIO().MouseDelta.x;
	if (bIsActive || bIsHovered)
		ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);
	ImGui::SameLine();
	
	if (ImGui::BeginChild("#settingContent", {0, 0}, true))
	{
		if (item_current_idx >= 0)
			configs[item_current_idx]->ShowWidgets();
	}
	ImGui::EndChild();
}

void Editor::GuiRender()
{
	static bool showStyleEditor = false;
	static bool showSettingsWindow = false;
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
			if (ImGui::MenuItem(LOCTEXT(EditorPreferences)))
				showSettingsWindow = true;
			ImGui::EndMenu();
		}
#if defined(_DEBUG) || defined(DEBUG)
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

	if (showSettingsWindow)
	{
		static ImVec2 settingsWindowSize = { viewport->WorkSize.x / 2.f, viewport->WorkSize.y / 2.f };
		ImGui::SetNextWindowSize(settingsWindowSize);
		if (ImGui::Begin(LOCTEXT(EditorPreferences), &showSettingsWindow, ImGuiWindowFlags_NoDocking))
		{
			ShowPreferencesWindow();
			ImGui::End();
		}
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

}
