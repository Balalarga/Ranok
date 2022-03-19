#pragma once

#include <Graphics/Gui/Gui.h>

#include <memory>

class Scene;
class RayMarchingView;


class Editor: public GuiBase
{
public:
    struct TextEditorTab
    {
        TextEditor tab;
        std::string name = "Unknown";
    };

    Editor();

    void Render() override;


    void AddTab(const std::string& text = "");
    void EraseTab(size_t id);
    void AddFuncWindow();


private:
    std::unique_ptr<Scene> _scene;
    RayMarchingView& _rayMarchView;
    std::vector<TextEditorTab> _textEditorTabs;

};
