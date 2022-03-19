#include "Editor.h"

#include <Graphics/Gui/Scene.h>
#include <Graphics/Opengl/RayMarchingView.h>
#include <Ranok/LanguageCore/Parser.h>
#include <Ranok/LanguageCore/CustomFunction.h>
#include <Ranok/LanguageCore/Functions.h>

#include "Utility/FileSystem.h"

static std::string Trim(const std::string& str,
                 const std::string& whitespace = " \t\n")
{
    const auto strBegin = str.find_first_not_of(whitespace);
    if (strBegin == std::string::npos)
        return ""; // no content

    const auto strEnd = str.find_last_not_of(whitespace);
    const auto strRange = strEnd - strBegin + 1;

    return str.substr(strBegin, strRange);
}

static std::string Reduce(const std::string& str,
                   const std::string& fill = " ",
                   const std::string& whitespace = " \t\n")
{
    // trim first
    auto result = Trim(str, whitespace);

    // replace sub ranges
    auto beginSpace = result.find_first_of(whitespace);
    while (beginSpace != std::string::npos)
    {
        const auto endSpace = result.find_first_not_of(whitespace, beginSpace);
        const auto range = endSpace - beginSpace;

        result.replace(beginSpace, range, fill);

        const auto newStart = beginSpace + fill.length();
        beginSpace = result.find_first_of(whitespace, newStart);
    }

    return result;
}


Editor::Editor():
    GuiBase("Editor##editor1"),
    _scene(new Scene()),
    _rayMarchView(_scene->AddObject<RayMarchingView>(_scene.get()))
{

    auto funcsFile = FileSystem::ReadAssetFile("ranokFunctions.txt");
    if (funcsFile.Ok())
    {
        int startId = 0;
        std::string data = Trim(funcsFile.Get());
        while (startId < data.size())
        {
            std::string cutted = data.substr(startId);
            Functions::AddCustom(CustomFunction::FromString(cutted, startId));
        }
    }

    AddTab();
}

void Editor::Render()
{
    ImGui::SetNextWindowPos({0, 0});
    ImGui::SetNextWindowSize(ImGui::GetWindowViewport()->Size);
    static int mainWindowFlags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
    if (ImGui::Begin(Name().c_str(), nullptr, mainWindowFlags))
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
                            _textEditorTabs[0].tab.SetText(fileData.Get());
                    }
                }

                if (ImGui::MenuItem("Save"))
                {
                    std::string filepath = FileDialog::GetFilepath(FileDialog::FileMode::Open, "*.txt");
                    std::string text = _textEditorTabs[0].tab.GetText();
                    if (!filepath.empty() && !text.empty())
                    {
                        FileSystem::WriteSomeFile(filepath, text);
                    }
                }

                if (ImGui::MenuItem("Compile"))
                {
                    Parser parser;
                    Program program = parser.Parse(Lexer::CreateFrom(_textEditorTabs[0].tab.GetText()));
                    _rayMarchView.SetModel(program);
                    _scene->NeedUpdate();
                }

                if (ImGui::MenuItem("Add function"))
                    ImGui::OpenPopup("Add function");

                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }

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
                    t.tab.Render();
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
    ImGui::ShowDemoWindow();
}

void Editor::AddTab(const std::string &text)
{
    TextEditor editor;
    editor.SetLanguageDefinition(TextEditor::LanguageDefinition::Ranok());
    _textEditorTabs.push_back({editor});
}

void Editor::EraseTab(size_t id)
{
    _textEditorTabs.erase(_textEditorTabs.begin() + id);
}

void Editor::AddFuncWindow()
{
//    if (ImGui::BeginPopupModal("Some Modal"))
//    {

//    }
//    ImGui::EndPopup();
}
