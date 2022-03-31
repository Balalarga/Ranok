#ifndef BLUEPRINTEDITOR_H
#define BLUEPRINTEDITOR_H


#include "Graphics/Gui/Gui.h"
#include "Utility/ImageStorage.h"

#include <imgui_node_editor.h>
#include <Ranok/LanguageCore/Program.h>


class BlueprintEditor: public GuiBase
{
public:
    enum class PinType
    {
        Flow,
        Bool,
        Int,
        Float,
        String,
        Object,
        Function,
        Delegate,
        Variable,
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
        Tree,
        Comment,
        Houdini
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

        Node(int id, const char* name, ImColor color = ImColor(128, 195, 248)):
            ID(id), Name(name), Color(color), Type(NodeType::Blueprint), Size(0, 0), Descr(""),
            CanDelete(true)
        {
        }
    };
    struct NodeIdLess
    {
        bool operator()(const ax::NodeEditor::NodeId& lhs, const ax::NodeEditor::NodeId& rhs) const
        {
            return lhs.AsPointer() < rhs.AsPointer();
        }
    };

    struct Link;
    struct Pin
    {
        ax::NodeEditor::PinId   ID;
        Node*     Node;
        std::string Name;
        PinType     Type;
        PinKind     Kind;
        int         LinkId;
        double      Value;

        Pin(int id, const char* name, PinType type):
            ID(id),
            Node(nullptr),
            Name(name),
            Type(type),
            Kind(PinKind::Input),
            Value(0),
            LinkId(-1)
        {
        }
    };

    struct Link
    {
        ax::NodeEditor::LinkId ID;

        ax::NodeEditor::PinId StartPinID;
        ax::NodeEditor::PinId EndPinID;

        ImColor Color;

        Link(ax::NodeEditor::LinkId id, ax::NodeEditor::PinId startPinId, ax::NodeEditor::PinId endPinId):
            ID(id), StartPinID(startPinId), EndPinID(endPinId), Color(255, 255, 255)
        {
        }
    };
    Node* FindNode(ax::NodeEditor::NodeId id);
    Link* FindLink(ax::NodeEditor::LinkId id);
    Pin* FindPin(const ax::NodeEditor::PinId& id);
    bool IsPinLinked(ax::NodeEditor::PinId id);
    void DrawPinIcon(const Pin &pin, bool connected, int alpha);

    static ImVec2 NodeIconsSize;

    BlueprintEditor();
    ~BlueprintEditor();

    Node* ContextMenu();
    void AddDefaultNodes();
    void Render() override;


    Program GetProgram();
    spExpression Iterate(Program& program, Pin* pin);


private:
    ImFont* _textFont;
    ax::NodeEditor::EditorContext* _context = nullptr;
    const ImageData _nodeHeaderBackground;

    std::vector<Link> _links;
    std::vector<Node> _nodes;
};

#endif // BLUEPRINTEDITOR_H
