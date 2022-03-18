#include "Editor.h"

#include <Graphics/Gui/Scene.h>
#include <Graphics/Gui/TextEditor/RanokTextEditor.h>
#include <Graphics/Opengl/RayMarchingView.h>


Editor::Editor():
    GuiBase("Editor##editor1"),
    _scene(new Scene()),
    _rayMarchView(_scene->AddObject<RayMarchingView>(_scene.get()))
{
    _textEditorTabs.push_back({std::make_unique<RanokTextEditor>()});
    _textEditorTabs.back().tab->SetTabSize(2);
}

void Editor::Render()
{
    ImGui::SetNextWindowPos({0, 0});
    ImGui::SetNextWindowSize(ImGui::GetWindowViewport()->Size);
    if (ImGui::Begin(Name().c_str(), nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove))
    {
        const auto parentWidth = ImGui::GetWindowContentRegionWidth();
        constexpr float widthCoef = 0.4;
        ImGui::BeginChild("EditorWindow", {parentWidth * widthCoef, 0});
        if (ImGui::BeginTabBar("Editor##editorTabs"))
        {
            for (int i = 0; i < _textEditorTabs.size();)
            {
                auto& t = _textEditorTabs[i];

                bool open = true;

                if (ImGui::BeginTabItem(t.name.c_str(), &open))
                {
                    t.tab->Render();
                }
                ImGui::EndTabItem();

                if (!open)
                    EraseTab(i);
                else
                    ++i;
            }
            if (ImGui::TabItemButton("+"))
                AddTab();
        }
        ImGui::EndTabBar();
        ImGui::EndChild();

        ImGui::SameLine();

        _scene->SetSize({parentWidth * (0.994f - widthCoef), 0});
        _scene->Render();
    }

    ImGui::End();
//    ImGui::ShowDemoWindow();
}

void Editor::AddTab(const std::string &text)
{
    _textEditorTabs.push_back({std::make_unique<RanokTextEditor>()});
}

void Editor::EraseTab(size_t id)
{
    _textEditorTabs.erase(_textEditorTabs.begin() + id);
}
