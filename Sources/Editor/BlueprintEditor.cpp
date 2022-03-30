#include "BlueprintEditor.h"

namespace ed = ax::NodeEditor;


BlueprintEditor::BlueprintEditor():
    _context(ed::CreateEditor())
{

}

BlueprintEditor::~BlueprintEditor()
{
    ed::DestroyEditor(_context);
}

void BlueprintEditor::Render()
{
    ed::SetCurrentEditor(_context);

    ed::Begin("My Editor");

    int uniqueId = 1;

    // Start drawing nodes.
    ed::BeginNode(uniqueId++);
    ImGui::Text("Node A");
        ed::BeginPin(uniqueId++, ed::PinKind::Input);
        ImGui::Text("In");
        ed::EndPin();
        ImGui::SameLine();
        ed::BeginPin(uniqueId++, ed::PinKind::Output);
        ImGui::Text("Out");
        ed::EndPin();
    ed::EndNode();

    ed::BeginNode(uniqueId++);
    ImGui::Text("Node B");
        ed::BeginPin(uniqueId++, ed::PinKind::Input);
        ImGui::Text("In");
        ed::EndPin();
        ImGui::SameLine();
        ed::BeginPin(uniqueId++, ed::PinKind::Output);
        ImGui::Text("Out");
        ed::EndPin();
    ed::EndNode();

    ed::End();
    ed::SetCurrentEditor(nullptr);
}
