#include "Editor.h"

#include <Graphics/Gui/Scene.h>
#include <Graphics/Opengl/RayMarchingView.h>
#include <Ranok/LanguageCore/Parser.h>
#include <Ranok/LanguageCore/CustomFunction.h>
#include <Ranok/LanguageCore/Functions.h>

#include "Utility/FileSystem.h"
#include "Utility/StringUtility.h"


Editor::Editor():
    GuiBase("Editor##editor1"),
    _scene(new Scene()),
    _rayMarchView(_scene->AddObject<RayMarchingView>(_scene.get()))
{

    auto funcsFile = FileSystem::ReadAssetFile("ranokFunctions.txt");
    if (funcsFile.Ok())
    {
        int startId = 0;
        std::string data = StringUtility::Trim(funcsFile.Get());
        while (startId < data.size())
        {
            std::string cutted = data.substr(startId);
            Functions::AddCustom(CustomFunction::FromString(cutted, startId));
        }
    }
}

void Editor::Render()
{
    auto& io = ImGui::GetIO();
    ImGui::SetNextWindowPos({0, 0});
    ImGui::SetNextWindowSize(io.DisplaySize);

    static int mainWindowFlags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDecoration;
    if (ImGui::Begin(Name().c_str(), NULL, mainWindowFlags))
    {
        EditorTab();
        ImGui::End();
    }

    ImGui::ShowDemoWindow();
}

void Editor::EditorTab()
{
    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Open"))
            {
                std::string filepath = FileDialog::GetFilepath(FileDialog::FileMode::Open, "*.txt");
                if (!filepath.empty())
                {
                    auto fileData = FileSystem::ReadSomeFile(filepath);
                    if (fileData.Ok())
                    {
                        auto tab = TextEditor::Tab::FromFilepath(filepath);
                        tab.window.SetText(fileData.Get());
                        _textEditor.AddTab(tab);
                    }
                }
            }

            if (ImGui::MenuItem("Save"))
            {
                std::string text = _textEditor.GetActiveTabText();
                if (!text.empty())
                {
                    std::string filepath = FileDialog::GetFilepath(FileDialog::FileMode::Save, "*.txt");
                    if (!filepath.empty())
                        FileSystem::WriteSomeFile(filepath, text);
                }
            }

            if (ImGui::MenuItem("Add function"))
                ImGui::OpenPopup("Add function");

            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }

    const auto parentWidth = ImGui::GetWindowContentRegionWidth();
    constexpr float widthCoef = 0.4;
    static ImVec2 childSize = {parentWidth * widthCoef, 0};
    if (ImGui::BeginChild("EditorFrame", childSize))
    {
        if (ImGui::Button("Compile"))
        {
            Parser parser;
            Program program = parser.Parse(Lexer::CreateFrom(_textEditor.GetActiveTabText()));
            _rayMarchView.SetModel(program);
            _scene->NeedUpdate();
        }


        _textEditor.SetSize(childSize);
        _textEditor.Render();

        ImGui::EndChild();
        ImGui::SameLine();
    }

    _scene->SetSize({parentWidth * (0.994f - widthCoef), 0});
    _scene->Render();
}

void Editor::ViewerTab()
{

}
