#include "Editor.h"

#include <Graphics/Gui/Scene.h>
#include <Graphics/Gui/TextEditor/RanokTextEditor.h>
#include <Graphics/Opengl/RayMarchingView.h>


Editor::Editor():
    GuiWindow("Editor##editor1")
{
    _scene = CreateChild<Scene>();
    _textEditor = CreateChild<RanokTextEditor>();

    _rayMarchView = new RayMarchingView(_scene);
    _scene->AddObject(_rayMarchView);
}

