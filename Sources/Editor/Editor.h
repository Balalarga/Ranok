#pragma once

#include <memory>

#include <Ranok/OpenCL/OpenclCalculator.h>

#include "Graphics/Opengl/VoxelObject.h"

#include "TextEditor.h"
#include "Blueprints/BlueprintEditor.h"
#include "Utility/ImageStorage.h"
#include "Settings.h"

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
    void BuildPopUp();


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
    Program _program;
    Space _space;
    VoxelObject* _voxelObject;
    unsigned _viewerSelectedImage;

    RayMarchingView* _rayMarchView;
    TextEditor _textEditor;
    OpenclCalculator _openclCalculator;
    BlueprintEditor _blueprintEditor;
    LinearGradient _imageGradient;
    FlatArray<MImage3D> _imageData;

    struct EditorTabSettings{
        Setting<glm::fvec3> BackgroundColor;
        Setting<glm::fvec3> ModelColor;
        void Render();

    } _editorTabSettings;
};
