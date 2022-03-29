#include "Editor.h"

#include <Graphics/Gui/Gui.h>
#include <Graphics/Opengl/GridObject.h>
#include <Graphics/Opengl/RayMarchingView.h>

#include <Ranok/LanguageCore/Parser.h>
#include <Ranok/LanguageCore/CustomFunction.h>
#include <Ranok/LanguageCore/Functions.h>
#include <Ranok/Utility/StringUtility.h>

#include "Utility/FileSystem.h"


Editor::Editor():
    GuiBase("Editor##editor1"),
    _rayMarchView(_scene.AddObject<RayMarchingView>(&_scene)),
    _tabButtons(2),
    _activeTab(0),
    _voxelObject(nullptr)
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

    TextEditor::Tab tab("New");
    tab.window.SetText(R"(args x, y, z; // default range [-1, 1];

x0 = 0;
y0 = 0;
z0 = 0;

r = 1;
s = r ^ 2 - (x - x0) ^ 2 -(y - y0) ^ 2 - (z - z0) ^ 2;

return s;)");
    _textEditor.AddTab(tab);

    SetupViewScene();
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
            if (ImGui::ImageButton((ImTextureID)i.imageData.id, {tabButtonsWidth, tabButtonsWidth}, {}, {1,1}, -1, ImVec4(0.2, 0.2, 0.2, 1.0)))
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
    constexpr unsigned customFuncNameLen = 256;
    static char customFuncName[customFuncNameLen];
    static TextEditWindow customFuncEditor;
    static bool customInit = false;
    if (!customInit)
    {
        customFuncEditor.SetLanguageDefinition(TextEditor::RanokLanguageDefinition());
        customInit = true;
    }

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
                _rayMarchView->SetModel(program);
                _scene.NeedUpdate();
            }
        }
    }

    ImGui::SameLine();
    if (ImGui::Button("Build"))
    {
        ImGui::OpenPopup("Build model data");
    }

    ImGui::SameLine();
    if (ImGui::Button("Add function"))
    {
        customFuncName[0] = '\0';
        customFuncEditor.SetText("");
        ImGui::OpenPopup("Create function");
    }


    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10, 10));

    bool unused_open = true;
    if (ImGui::BeginPopupModal("Build model data", &unused_open, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::BeginChild("PathContainer", ImVec2(0, 40));
        static std::string pathToSave(512, ' ');
        ImGui::InputText("##resPath1", &pathToSave[0], 512, ImGuiInputTextFlags_ReadOnly | ImGuiInputTextFlags_AutoSelectAll);
        ImGui::SameLine();
        if (ImGui::Button("Open"))
        {
            std::string filepath = FileDialog::GetFilepath(FileDialog::FileMode::Save);
            if (!filepath.empty())
                pathToSave = filepath;
        }
        ImGui::EndChild();

        static CalculateTarget calculateTarget;
        static std::string selectedText = "Model";
        if (ImGui::BeginCombo("Compute type", selectedText.c_str()))
        {
            if (ImGui::Selectable("Model"))
            {
                selectedText = "Model";
                calculateTarget = CalculateTarget::Model;
            }
            if (ImGui::Selectable("M-Image"))
            {
                selectedText = "M-Image";
                calculateTarget = CalculateTarget::Image;
            }

            ImGui::EndCombo();
        }
        constexpr int minDepth = 5; // Opencl local group size
        constexpr int maxDepth = 15;
        static int recursionDepth = minDepth;
        std::string depthLabel = std::to_string((int)pow(2, recursionDepth)) + " voxels/axis";
        if (ImGui::InputInt(depthLabel.c_str(), &recursionDepth, 1, 100, ImGuiInputTextFlags_AlwaysInsertMode))
        {
            if (recursionDepth > maxDepth)
                recursionDepth = maxDepth;
            else if(recursionDepth < minDepth)
                recursionDepth = minDepth;
        }

        static bool enableBatching = false;
        ImGui::Checkbox("Enable batching", &enableBatching);

        constexpr int batchMin = 10;
        static int batchSize = batchMin;
        if (enableBatching)
        {
            std::string batchLabel = std::to_string((pow(2, batchSize) * (calculateTarget  == CalculateTarget::Model ? sizeof(char) : sizeof(double) * 5) ) / 1024.f / 1024.f) + " MB";
            if (ImGui::InputInt(batchLabel.c_str(), &batchSize, 1, 100, ImGuiInputTextFlags_AlwaysInsertMode))
            {
                if (batchSize < batchMin)
                    batchSize = batchMin;
            }
        }


        if (ImGui::Button("Start"))
        {
            ImGui::CloseCurrentPopup();
            Parser parser;
            Program program = parser.Parse(Lexer::CreateFrom(_textEditor.GetActiveTabText()));
            auto args = program.GetAllOf<ArgumentExpression>();
            if (args.size() == 3)
            {
                std::vector<ArgumentExpression::Range> ranges{args[0]->range, args[1]->range, args[2]->range};
                std::vector<double> startPoint{ranges[0].min, ranges[1].min, ranges[2].min};
                std::vector<double> spaceSize{ranges[0].max - ranges[0].min, ranges[1].max - ranges[1].min, ranges[2].max - ranges[2].min};
                _space.SetSize(spaceSize);
                _space.SetPartition(pow(2, recursionDepth));
                _space.SetStartPoint(startPoint);
                auto calculateCallback = [this, targetCopy = calculateTarget](size_t start, size_t count)
                {
                    std::ios_base::openmode mode = std::ios_base::openmode::_S_out;
                    if (start != 0)
                        mode = std::ios_base::app;

                    std::ofstream file(pathToSave, mode);
                    if (file)
                    {
                        bool ok = false;
                        if (start == 0)
                        {
                            file << _space;
                            file << static_cast<uint8_t>(targetCopy);
                        }

                        if (targetCopy == CalculateTarget::Model)
                            ok = _openclCalculator.GetModel().WritePart(file, count);
                        else
                            ok = _openclCalculator.GetImage().WritePart(file, count);

                        if (!ok)
                            std::cout << "Write error\n";

                        file.close();
                    }
                };
                bool calculateStatus = _openclCalculator.Calculate(calculateTarget,
                                                                   program,
                                                                   _space,
                                                                   calculateCallback,
                                                                   enableBatching ? pow(2, batchSize) : 0);
                if (!calculateStatus)
                    std::cout << "Some calculate error\n";
            }
        }

        ImGui::EndPopup();
    }

    unused_open = true;
    if (ImGui::BeginPopupModal("Compile error", &unused_open, ImGuiWindowFlags_AlwaysAutoResize))
    {
        ImGui::Text("%s", compileError.c_str());

        ImGui::EndPopup();
    }

    unused_open = true;
    if (ImGui::BeginPopupModal("Create function", &unused_open, ImGuiWindowFlags_AlwaysAutoResize))
    {
        static ImVec4 color(1.f, 1.f, 1.f, 1.f);
        static bool functionExists = false;
        ImGui::PushStyleColor(ImGuiCol_Text, color);
        customFuncEditor.SetLanguageDefinition(TextEditor::RanokLanguageDefinition());
        if (ImGui::InputText("Function name", customFuncName, customFuncNameLen))
        {
            if (Functions::Find(customFuncName) || Functions::FindCustom(customFuncName))
            {
                color = ImVec4(1.f, 0.2f, 0.2f, 1.f);
                functionExists = true;
            }
            else
            {
                color = ImVec4(1.f, 1.f, 1.f, 1.f);
                functionExists = false;
            }
        }
        ImGui::PopStyleColor();

        customFuncEditor.Render("Custom func code", ImVec2(500, 300), true);

        static std::string functionCreationError = "";
        if (ImGui::Button("Create"))
        {
            if (functionExists)
            {
                ImGui::OpenPopup("Function error");
                functionCreationError = "Function named " + std::string(customFuncName) + " already exists";
            }
            else
            {
                CustomFunction func(customFuncName, customFuncEditor.GetText());
                if (!func.Root())
                {
                    ImGui::OpenPopup("Function error");
                    functionCreationError = "Function code error, please check it";
                }
                else
                {
                    Functions::AddCustom(func);
                    _textEditor.UpdateLanguageDef();
                    ImGui::CloseCurrentPopup();
                }
            }
        }
        bool unusedopen = true;
        if (ImGui::BeginPopupModal("Function error", &unusedopen, ImGuiWindowFlags_AlwaysAutoResize))
        {
            ImGui::TextUnformatted(functionCreationError.c_str());
            ImGui::EndPopup();
        }

        ImGui::EndPopup();
    }
    ImGui::PopStyleVar();

    _textEditor.SetSize(childSize);
    _textEditor.Render();

    ImGui::EndChild();
    ImGui::SameLine();

    _scene.Render();
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
                std::string filepath = FileDialog::GetFilepath(FileDialog::FileMode::Open);
                if (!filepath.empty() && FileSystem::HasFile(filepath))
                {
                    _modelData.Clear();
                    _imageData.Clear();
                    std::ifstream file(filepath);

                    file >> _space;

                    uint8_t target;
                    file >> target;
                    _lastTarget = (CalculateTarget)target;

                    size_t dataSize = _space.GetTotalPartition();
                    if (_lastTarget == CalculateTarget::Model)
                    {
                        _modelData.Resize(dataSize);
                        _modelData.ReadSome(file, dataSize);
                    }
                    else
                    {
                        _imageData.Resize(dataSize);
                        _imageData.ReadSome(file, dataSize);
                    }
                    file.close();

                    if (!_voxelObject)
                    {
                        if (_lastTarget == CalculateTarget::Model)
                            _voxelObject = _imageScene.AddObject<VoxelObject>(&_imageScene, _space, _modelData);
                        else
                            _voxelObject = _imageScene.AddObject<VoxelObject>(&_imageScene, _space, _imageData);
                    }
                    else
                    {
//                        _voxelObject->Recreate(BufferInfo(resData, dataSize, GL_POINTS));
                    }
                    _imageScene.NeedUpdate();
                }
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

void Editor::SetupViewScene()
{
    struct Vertex
    {
        float x, y, z;
        glm::fvec4 color;
    };
    _imageScene.SetBackgroundColor({0.8, 0.8, 0.8, 1.0});

    Vertex data[]{
        { 0, 0, 0, {0.7, 0.3, 0.3, 1.0} },
        { 0, 1, 1, {0.7, 0.3, 0.3, 1.0} },
        { 0, 1, 0, {0.7, 0.3, 0.3, 1.0} },
    };
    int count = sizeof(data)/sizeof(data[0]);
    _imageScene.AddObject<Renderable>(&_imageScene, new Shader(), BufferInfo(data, count, GL_TRIANGLE_FAN));
    _imageScene.GetCamera().MouseSensitivity = 0.1f;
}

void Editor::ActivateTab(unsigned n)
{
    _activeTab = n;
}
