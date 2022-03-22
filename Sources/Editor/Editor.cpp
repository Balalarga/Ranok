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
    _rayMarchView(_scene.AddObject<RayMarchingView>(&_scene)),
    _tabButtons(2),
    _activeTab(0)
{
    TabButton& textEditorBtn = _tabButtons.at(0);
    textEditorBtn.imageData = ImageStorage::Get().Load("editIcon", "Images/editIcon.png");
    textEditorBtn.pressed = [this](){
        ActivateTab(0);
    };
    textEditorBtn.render = std::bind(&Editor::EditorTab, this);

    TabButton& viewBtn = _tabButtons.at(1);
    viewBtn.imageData = ImageStorage::Get().Load("cameraIcon", "Images/cameraIcon.png");
    viewBtn.pressed = [this](){
        ActivateTab(1);
    };
    viewBtn.render = std::bind(&Editor::ViewerTab, this);

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
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
    if (ImGui::Begin(Name().c_str(), NULL, mainWindowFlags))
    {
        constexpr int tabButtonsWidth = 32;
        ImGui::BeginChild("EditorWindowsTabs", {tabButtonsWidth, 0}, true);
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
        for (auto& i: _tabButtons)
        {
            if (ImGui::ImageButton((ImTextureID)i.imageData.id, {tabButtonsWidth, tabButtonsWidth}))
                i.pressed();
        }
        ImGui::PopStyleVar(1);
        ImGui::EndChild();
        ImGui::SameLine();

        _tabButtons.at(_activeTab).render();
        ImGui::End();
    }
    ImGui::PopStyleVar(1);
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
    constexpr float widthCoef = 0.34;
    ImVec2 childSize = {parentWidth * widthCoef, 0};

    static std::string compileError;

    ImGui::BeginChild("EditorFrameChild", childSize);
    if (ImGui::Button("Compile"))
    {
        Lexer lexer = Lexer::CreateFrom(_textEditor.GetActiveTabText());
        Parser parser;
        if (_textEditor.GetActiveTabText().empty())
        {
            compileError = "Text is empty";
            ImGui::OpenPopup("Compile error");
        }
        else if (!lexer.Error().empty())
        {
            compileError = lexer.Error();
            ImGui::OpenPopup("Compile error");
        }
        else
        {
            Program program = parser.Parse(lexer);
            if (!program.Root())
            {
                compileError = "Program created with error\nCheck your's code";
                ImGui::OpenPopup("Compile error");
            }
            else if (!parser.Error().empty())
            {
                compileError = parser.Error();
                ImGui::OpenPopup("Compile error");
            }
            else
            {
                _rayMarchView.SetModel(program);
                _scene.NeedUpdate();
            }
        }
    }

    ImGui::SameLine();
    if (ImGui::Button("Build"))
    {
    }

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10, 10));
    bool unused_open = true;
    if (ImGui::BeginPopupModal("Compile error", &unused_open, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Text("%s", compileError.c_str());

        ImGui::EndPopup();
    }
    ImGui::PopStyleVar();


    _textEditor.SetSize(childSize);
    _textEditor.Render();

    ImGui::EndChild();
    ImGui::SameLine();

    _scene.Render();

    ImGui::ShowDemoWindow();
}

void Editor::ViewerTab()
{
    ImGuiIO& io = ImGui::GetIO();
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10, 10));
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(3, 3));

    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Open"))
            {
            }

            if (ImGui::MenuItem("Save image"))
            {
            }

            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }


    const auto parentWidth = ImGui::GetWindowContentRegionWidth();
    constexpr float widthCoef = 0.34;
    ImVec2 childSize = {parentWidth * widthCoef, 0};

    ImGui::BeginChild("ViewsTabControls", childSize);

    static float xMin, xMax;
    static float yMin, yMax;
    static float zMin, zMax;
    if (ImGui::DragFloatRange2("Ox", &xMin, &xMax, .001f, -1, 1))
    {
        // update scene
    }
    if (ImGui::DragFloatRange2("Oy", &yMin, &yMax, .001f, -1, 1))
    {
        // update scene
    }
    if (ImGui::DragFloatRange2("Oz", &zMin, &zMax, .001f, -1, 1))
    {
        // update scene
    }


    static int selectedImage = 0;
    constexpr int imageSize = 3 + 2;
    if (ImGui::BeginCombo("Image", ("C"+std::to_string(selectedImage)).c_str()))
    {
        for (int i = 0; i < imageSize; ++i)
        {
            if (ImGui::Selectable(("C"+std::to_string(i)).c_str()))
                selectedImage = i;
        }
        ImGui::EndCombo();
    }

    ImGui::EndChild();
    ImGui::PopStyleVar(2);

    ImGui::SameLine();

    _imageScene.Render();
}

void Editor::ActivateTab(unsigned n)
{
    _activeTab = n;
}
