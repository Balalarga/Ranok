#pragma once

#include <memory>

#include <Ranok/Core/OpenclCalculator.h>

#include "TextEditor.h"
#include "Utility/ImageStorage.h"

class Scene;
class RayMarchingView;


class Editor: public GuiBase
{
public:
    Editor();

    void Render() override;

    void TryOpenPopup();

    void EditorTab();
    void ViewerTab();

    void ActivateTab(unsigned n);


private:
    struct TabButton
    {
        ImageData imageData;
        std::function<void()> pressed;
        std::function<void()> render;
    };
    std::vector<TabButton> _tabButtons;
    unsigned _activeTab;

    Scene _scene;
    Scene _imageScene;
    Space _space;
    RayMarchingView& _rayMarchView;
    TextEditor _textEditor;
    OpenclCalculator _openclCalculator;
};
