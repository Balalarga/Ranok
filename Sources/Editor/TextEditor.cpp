#include "TextEditor.h"
#include "Utility/FileSystem.h"
#include "Utility/StringUtility.h"


#include <Ranok/LanguageCore/Functions.h>


std::string TextEditor::LanguageCustomFunctionsFilepath = "ranokFunctions.txt";
static unsigned tabCounter = 0;

TextEditor::TextEditor():
    GuiBase("TextEditor"),
    _activeTab(-1)
{
}

void TextEditor::Render()
{
    ImGui::BeginChild(Name().c_str(), Size());
    if (ImGui::BeginTabBar("##TextEditorTabBar", ImGuiTabBarFlags_AutoSelectNewTabs))
    {
        if (ImGui::TabItemButton("+", ImGuiTabItemFlags_Trailing | ImGuiTabItemFlags_NoTooltip))
            AddTab(Tab("New"));

        for (size_t i = 0; i < _tabs.size();)
        {
            auto& tab = _tabs[i];

            bool opened = true;
            if (ImGui::BeginTabItem(tab.id.c_str(), &opened))
            {
                _activeTab = i;
                tab.window.Render(tab.title.c_str());
                ImGui::EndTabItem();
            }

            if (!opened)
                RemoveTab(i);
            else
                ++i;
        }

        ImGui::EndTabBar();
    }
    ImGui::EndChild();
}

void TextEditor::AddTab(const Tab& tab)
{
    if (_tabs.size() > _maxTabs)
        return;

    std::string tabId;
    unsigned id = _tabs.size();
    auto founded = _tabs.end();
    do
    {
        if (id != 0)
            tabId = tab.title + std::to_string(id++);
        else
            tabId = tab.title;

        founded = std::find_if(_tabs.begin(), _tabs.end(), [&tabId](const Tab& tab){
                    return tab.id == tabId;
                });
    } while (founded != _tabs.end());

    _tabs.push_back(tab);

    auto& last = _tabs.back();
    last.id = tabId;
    last.window.SetLanguageDefinition(RanokLanguageDefinition());
}

void TextEditor::RemoveTab(unsigned id)
{
    if (id >= _tabs.size())
        return;

    _tabs.erase(_tabs.begin() + id);
}

const TextEditor::Tab *TextEditor::GetActiveTab()
{
    if (_activeTab >= 0)
        return &_tabs.at(_activeTab);

    return nullptr;
}

std::string TextEditor::GetActiveTabText()
{
    if (auto tab = GetActiveTab())
        return tab->window.GetText();

    return "";
}

const TextEditWindow::LanguageDefinition &TextEditor::RanokLanguageDefinition()
{
    static bool inited = false;
    static TextEditWindow::LanguageDefinition langDef = TextEditWindow::LanguageDefinition::GLSL();
    if (!inited)
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

        static std::string keywords[] = {
            "var", "args", "arg", "variable", "argument", "arguments", "return", "const", "constant"
        };
        static std::string baseIds[] =
        {
            "abs", "sqrt", "sin", "cos", "tan", "arctan", "arcsin", "arccos", "cosh", "sinh",
            "tanh", "exp", "ln", "log", "log10", "log2", "ceil", "floor"
        };

        langDef.mKeywords.clear();
        for (auto& k : keywords)
            langDef.mKeywords.insert(k);

        // {description, name}
        std::vector<std::pair<std::string, std::string>> identifiers;

        langDef.mIdentifiers.clear();
        for (auto& k : baseIds)
        {
            identifiers.push_back({"Build-in function", k});
        }

        for (auto& f: Functions::GetAll())
        {
            std::string name = f.name;
            identifiers.push_back({"Build-in function", f.name});
        }

        for (auto& f: Functions::GetAllCustoms())
        {
            std::string name = f.Info().name;
            identifiers.push_back({"User-define function", name});
        }


        for (auto& k : identifiers)
        {
            TextEditWindow::Identifier id;
            id.mDeclaration = k.first;
            langDef.mIdentifiers.insert(std::make_pair(k.second, id));
        }

        langDef.mSingleLineComment = "//";

        langDef.mCaseSensitive = false;
        langDef.mAutoIndentation = true;

        langDef.mName = "Ranok";

        inited = true;
    }
    return langDef;
}

TextEditor::Tab TextEditor::Tab::FromFilepath(const std::string &filepath)
{
    Tab tab(filepath, filepath);

    int folderSplitter = filepath.find_last_of("/");
    int extensionSplitter = filepath.find_last_of(".");

    if (folderSplitter == -1)
        folderSplitter = filepath.find_last_of("\\");

    if (folderSplitter != -1)
        tab.title = filepath.substr(folderSplitter + 1, extensionSplitter - folderSplitter - 1);
    else
        tab.title = filepath.substr(0, extensionSplitter);

    return tab;
}
