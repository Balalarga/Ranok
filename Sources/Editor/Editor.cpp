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


class EditorConfigs: public IConfig
{
public:
	EditorConfigs(): IConfig("Editor/EditorConfigs", true)
	{
	}
	
	void Serialize(JsonArchive& archive) override
	{
		archive.Serialize("DefaultLayoutIniPath", defaultLayoutIni);
		archive.Serialize("WindowWidth", windowSize.x);
		archive.Serialize("WindowHeight", windowSize.y);
	}

	std::string defaultLayoutIni;
	glm::uvec2 windowSize{1280, 720};
};
std::shared_ptr<EditorConfigs> editorConfigs;


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
	editorConfigs = ConfigManager::Instance().CreateConfigs<EditorConfigs>();
    AddGuiLayer(GuiLayer([this] { GuiRender(); }));
	TryLoadDefaultLayout();
	Resize(editorConfigs->windowSize);
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
		if (ImGui::BeginMenu(LOCTEXT(EditorSettinsMenu)))
		{
			if (ImGui::MenuItem("Save as default layout"))
				ImGui::SaveIniSettingsToDisk(editorConfigs->defaultLayoutIni.c_str());
			ImGui::EndMenu();
		}
		
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
}
 