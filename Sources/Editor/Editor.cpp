#include "Editor.h"
#include "GuiWrap/WMenuBar.h"
#include "GuiWrap/WTexture.h"
#include "imgui.h"

#include "GuiWrap/WLogs.h"
#include "GuiWrap/WRawWidget.h"

#include "Localization/LocalizationSystem.h"

namespace Ranok
{
DEFINELOCALETEXT(PluginsMenu, "Plugins")

ModuleSystem<IEditorModule> Editor::EditorSystem;

Editor& Editor::Instance()
{
    static Editor editor;
    return editor;
}

Editor::Editor()
{
	EditorSystem.Init();

    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    AddGuiLayer(GuiLayer([this] { GuiRender(); }));
}

void Editor::GuiRender()
{
    ImGui::PushStyleColor(ImGuiCol_DockingEmptyBg, ImVec4(0.02f, 0.02f, 0.02f, 1.f));
        
    constexpr ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking |
        ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoBackground;
    
    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);
    ImGui::SetNextWindowViewport(viewport->ID);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("Editor", nullptr, window_flags);
    ImGui::PopStyleVar();

    ImGui::PopStyleVar(2);

    ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
    ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);

    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu(GETLOCALETEXT(PluginsMenu).c_str()))
        {
            EditorSystem.EnumerateModules([](IEditorModule*)
            {
                
            });
            
            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }
    static WLogs logWindow;
    logWindow.Render();
    
    static WTexture texture(glm::uvec2(800, 600));
    if (ImGui::Begin("Viewport"))
    {
        texture.Render();
    }
    ImGui::End();

    ImGui::End();
    ImGui::PopStyleColor();
}

void Editor::AddModuleMenuItem(ModuleMenuItem&& item)
{
    _moduleMenuItems.push_back(item);
}

IEditorModule::IEditorModule(const std::string& moduleName):
    moduleName(moduleName)
{
    Editor::Instance().AddGuiLayer({[this]{ RenderGui(); }});
}

void IEditorModule::RenderGui()
{
    
}
}
