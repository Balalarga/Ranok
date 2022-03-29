#ifndef TEXTEDITOR_H
#define TEXTEDITOR_H

#include "Graphics/Gui/Gui.h"


class TextEditor: public GuiBase
{
public:
    struct Tab
    {
        Tab(const std::string& title, const std::string& filepath = ""):
            title(title),
            filepath(filepath) {}

        static Tab FromFilepath(const std::string& filepath);

        std::string title;
        std::string filepath;
        std::string id;

        TextEditWindow window;
    };

    TextEditor();
    void Render() override;

    void AddTab(const Tab& tab);
    void RemoveTab(unsigned id);

    const Tab* GetActiveTab();
    std::string GetActiveTabText();


    static const TextEditWindow::LanguageDefinition& RanokLanguageDefinition(bool forceUpdate = false);
    static std::string LanguageCustomFunctionsFilepath;


private:
    std::vector<Tab> _tabs;
    int _activeTab;

    static constexpr unsigned _maxTabs = 20;
};

#endif // TEXTEDITOR_H
