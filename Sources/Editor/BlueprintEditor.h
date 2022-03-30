#ifndef BLUEPRINTEDITOR_H
#define BLUEPRINTEDITOR_H


#include "Graphics/Gui/Gui.h"

#include <imgui_node_editor.h>




class BlueprintEditor: public GuiBase
{
public:
    BlueprintEditor();
    ~BlueprintEditor();

    void Render() override;


private:
    ax::NodeEditor::EditorContext* _context = nullptr;
};

#endif // BLUEPRINTEDITOR_H
