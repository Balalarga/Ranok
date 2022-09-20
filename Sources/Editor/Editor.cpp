#include "Editor.h"
#include "GuiWrap/WMenuBar.h"
#include "imgui.h"
#include "Localization/LocalizationSystem.h"

namespace Ranok
{
DEFINELOCALETEXT(PluginsMenu, "Plugins")

ModuleSystem<IEditorModule> Editor::EditorSystem;

ImGuiWindowFlags sMainWindowFlags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDecoration;

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
}

void Editor::GuiRender()
{
    _mainWindow.Render();
    EditorSystem.EnumerateModules([this](IEditorModule *mod)
    {
        if (mod->bHasGui)
            mod->Render();
    });
}
}
