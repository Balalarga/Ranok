#pragma once

#include <memory>

#include "TextEditor.h"

class Scene;
class RayMarchingView;


class Editor: public GuiBase
{
public:
    Editor();

    void Render() override;


    void EditorTab();
    void ViewerTab();

private:
    std::unique_ptr<Scene> _scene;
    RayMarchingView& _rayMarchView;
    TextEditor _textEditor;
};
