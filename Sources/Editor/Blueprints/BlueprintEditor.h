#ifndef BLUEPRINTEDITOR_H
#define BLUEPRINTEDITOR_H


#include "Graphics/Gui/Gui.h"
#include "Utility/ImageStorage.h"

#include <imgui_node_editor.h>
#include <Ranok/LanguageCore/CustomFunction.h>
#include <Ranok/LanguageCore/Program.h>
#include <set>

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

        Node(ax::NodeEditor::NodeId id, const char* name, ImColor color = ImColor(128, 195, 248)):
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
        Node*       Node;
        std::string Name;
        PinType     Type;
        PinKind     Kind;
        std::vector<Link*> Linker;
        std::vector<double> Values;

        Pin() = default;
        Pin(int id, const char* name, PinType type, PinKind kind = PinKind::Input):
            ID(id),
            Node(nullptr),
            Name(name),
            Type(type),
            Kind(kind),
            Values({0})
        {
        }
        Pin(ax::NodeEditor::PinId id, const char* name, PinType type, PinKind kind = PinKind::Input):
            ID(id),
            Node(nullptr),
            Name(name),
            Type(type),
            Kind(kind),
            Values({0})
        {
        }
    };

    struct Link
    {
        ax::NodeEditor::LinkId ID;

        Pin& StartPin;
        Pin& EndPin;

        ImColor Color;

        ~Link()
        {
            auto bIt = std::find(StartPin.Linker.begin(), StartPin.Linker.end(), this);
            if (bIt != StartPin.Linker.end())
                StartPin.Linker.erase(bIt);

            auto eIt = std::find(EndPin.Linker.begin(), EndPin.Linker.end(), this);
            if (eIt != EndPin.Linker.end())
                EndPin.Linker.erase(eIt);
        }

        Link(ax::NodeEditor::LinkId id, Pin& startPin, Pin& endPin):
            ID(id), StartPin(startPin), EndPin(endPin), Color(255, 255, 255)
        {
            StartPin.Linker.push_back(this);
            EndPin.Linker.push_back(this);
        }
        Link& operator=(const Link& oth)
        {
            this->ID = oth.ID;
            this->StartPin = oth.StartPin;
            this->EndPin = oth.EndPin;
            this->Color = oth.Color;
            return *this;
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

    void Save(const std::string& filepath);
    void Open(const std::string& filepath);


    Program GetProgram();
    spExpression Iterate(Program& program, Pin& pin);


private:
    ImFont* _textFont;
    ax::NodeEditor::EditorContext* _context = nullptr;
    const ImageData _nodeHeaderBackground;

    std::vector<std::unique_ptr<Link>> _links;
    std::vector<Node> _nodes;
    BlueprintEditor::Node *RenderContextCustomFunc(const CustomFunction &func, ImGuiTextFilter &Filter, int level = 0);
};

#endif // BLUEPRINTEDITOR_H
