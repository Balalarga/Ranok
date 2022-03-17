#pragma once

#include <Graphics/Gui/Base/GuiItem.h>

class Scene;
class RanokTextEditor;
class RayMarchingView;


class Editor: public GuiWindow
{
public:
    Editor();


private:
    Scene* _scene;
    RanokTextEditor* _textEditor;
    RayMarchingView* _rayMarchView;
};
