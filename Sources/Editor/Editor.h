#pragma once

#include <Graphics/Gui/Base/GuiItem.h>
#include <memory>

class Scene;
class RanokTextEditor;
class RayMarchingView;


class Editor: public GuiBase
{
public:
    struct TextEditorTab
    {
        std::unique_ptr<RanokTextEditor> tab;
        std::string name = "Unknown";
    };

    Editor();

    void Render() override;


    void AddTab(const std::string& text = "");
    void EraseTab(size_t id);


private:
    std::unique_ptr<Scene> _scene;
    RayMarchingView& _rayMarchView;
    std::vector<TextEditorTab> _textEditorTabs;

};
