#include "Editor.h"
#include "GuiWrap/WMenuBar.h"
#include "GuiWrap/WTexture.h"
#include "imgui.h"

#include "GuiWrap/WLogs.h"

#include "Localization/LocalizationSystem.h"

namespace Ranok
{
DEFINELOCALETEXT(PluginsMenu, "Plugins")

ModuleSystem<IEditorModule> Editor::EditorSystem;

ImGuiWindowFlags sMainWindowFlags = ImGuiWindowFlags_MenuBar |
    ImGuiWindowFlags_NoDecoration |
    ImGuiWindowFlags_NoResize |
    ImGuiWindowFlags_NoBringToFrontOnFocus;

Editor::Editor():
    _mainWindow("Editor", sMainWindowFlags)
{
	EditorSystem.Init();

    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    AddGuiLayer(GuiLayer([this] { GuiRender(); }));

    auto& mainMenuBar = _mainWindow.Add<WMenuBar>();
    auto& pluginsMenu = mainMenuBar.Add<WMenu>(GETLOCALETEXT(PluginsMenu));
    EditorSystem.EnumerateModules([this, &pluginsMenu](IEditorModule *mod)
    {
        auto& menuItem = pluginsMenu.Add<WMenuCheckItem>(mod->Title(), [mod](bool state)
        {
            if (state)
                mod->Open();
            else
                mod->Close();
        });
        mod->SetMenuItemPtr(&menuItem);
    });

    _mainWindow.Add<WTexture>(glm::uvec2(800, 600));
    _logWindow = &_mainWindow.Add<WLogs>();
}

void Editor::GuiRender()
{
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->Pos);
    ImGui::SetNextWindowSize(viewport->Size);
    ImGui::SetNextWindowViewport(viewport->ID);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    _mainWindow.Render();
    ImGui::PopStyleVar();
    EditorSystem.EnumerateModules([this](IEditorModule *mod)
    {
        if (mod->bHasGui)
            mod->Render();
    });
}
}
