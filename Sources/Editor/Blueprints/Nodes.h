#pragma once

#include <imgui_node_editor.h>

#include "builders.h"
#include "widgets.h"

#include <vector>
#include <string>


namespace Blueprint
{

enum class PinType
{
    Flow,
    Bool,
    Float,
    Variable,
    Array
};
enum class PinKind
{
    Output,
    Input
};
enum class NodeType
{
    Blueprint,
    Simple,
    Comment
};

struct Pin;
struct Node
{
    ax::NodeEditor::NodeId ID;
    std::string Name;
    ImColor Color;
    std::vector<Pin> Inputs;
    std::vector<Pin> Outputs;
    NodeType Type;
    ImVec2 Size;
    std::string Descr;
    bool CanDelete;

    std::string State;
    std::string SavedState;

    Node(int id, const char* name, ImColor color = ImColor(128, 195, 248));
};
struct NodeIdLess
{
    bool operator()(const ax::NodeEditor::NodeId& lhs, const ax::NodeEditor::NodeId& rhs) const;
};
struct Link;
struct Pin
{
    ax::NodeEditor::PinId   ID;
    Node*       Node;
    std::string Name;
    PinType     Type;
    PinKind     Kind;
    Link*       Link;
    double      Value;

    Pin(int id, const char* name, PinType type);

    inline bool IsLinked() { return Link != nullptr; }
};

struct Link
{
    ax::NodeEditor::LinkId ID;

    ax::NodeEditor::PinId StartPinID;
    ax::NodeEditor::PinId EndPinID;

    ImColor Color;

    Link(ax::NodeEditor::LinkId id, ax::NodeEditor::PinId startPinId, ax::NodeEditor::PinId endPinId);
};

ImColor GetIconColor(PinType type);

void DrawPinIcon(const Pin& pin, bool connected, int alpha, ImVec2 IconsSize = {20, 20});


}
