#pragma once

#include <memory>

#include <Ranok/Core/OpenclCalculator.h>

#include "Graphics/Opengl/VoxelObject.h"

#include "TextEditor.h"
#include "Blueprints/BlueprintEditor.h"
#include "Utility/ImageStorage.h"

class Scene;
class RayMarchingView;


class Editor: public GuiBase
{
public:
    Editor();
    ~Editor();

    void Render() override;

    void ActivateTab(unsigned n);


    void EditorTab();
    void ViewerTab();
    void BlueprintTab();

    void SetupViewScene();


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

    Space _space;
    Scene _imageScene;
    CalculateTarget _lastTarget;
    FlatArray<char> _modelData;
    FlatArray<double> _imageData;
    VoxelObject* _voxelObject;

    RayMarchingView* _rayMarchView;
    TextEditor _textEditor;
    OpenclCalculator _openclCalculator;
    BlueprintEditor _blueprintEditor;
};
