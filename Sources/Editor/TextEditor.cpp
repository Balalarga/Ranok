#include "TextEditor.h"
#include "Utility/FileSystem.h"

#include <Ranok/Utility/StringUtility.h>
#include <Ranok/LanguageCore/Functions.h>


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
    if (tab.window.GetText().size() == 1)
        last.window.SetText(R"(args s[3](1, 1, 1);

return ;)");
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

void TextEditor::UpdateLanguageDef()
{
    RanokLanguageDefinition(true);
    for (auto& i: _tabs)
        i.window.SetLanguageDefinition(RanokLanguageDefinition());
}

const TextEditWindow::LanguageDefinition &TextEditor::RanokLanguageDefinition(bool forceUpdate)
{
    static bool inited = false;
    static TextEditWindow::LanguageDefinition langDef = TextEditWindow::LanguageDefinition::GLSL();
    if (!inited || forceUpdate)
    {
        langDef = TextEditWindow::LanguageDefinition::GLSL();

        static std::string keywords[] = {
            "var", "args", "return",
        };

        langDef.mKeywords.clear();
        for (auto& k : keywords)
            langDef.mKeywords.insert(k);

        // {description, name}
        std::vector<std::pair<std::string, std::string>> identifiers;

        langDef.mIdentifiers.clear();
        for (auto& f: Functions::GetAll())
            identifiers.push_back({f.Desc(), f.Name()});

        for (auto& f: Functions::GetAllCustoms())
            identifiers.push_back({f.Info().Desc(), f.Info().Name()});


        for (auto& k : identifiers)
        {
            TextEditWindow::Identifier id;
            id.mDeclaration = k.first;
            langDef.mIdentifiers.insert(std::make_pair(k.second, id));
        }

        langDef.mSingleLineComment = "//";

        langDef.mCaseSensitive = true;
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
