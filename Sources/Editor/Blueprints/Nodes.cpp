#include "Nodes.h"



Blueprint::Node::Node(int id, const char *name, ImColor color):
    ID(id), Name(name), Color(color), Type(NodeType::Blueprint), Size(0, 0), Descr(""),
    CanDelete(true)
{
}

bool Blueprint::NodeIdLess::operator()(const ax::NodeEditor::NodeId &lhs, const ax::NodeEditor::NodeId &rhs) const
{
    return lhs.AsPointer() < rhs.AsPointer();
}

Blueprint::Pin::Pin(int id, const char *name, PinType type):
    ID(id), Node(nullptr), Name(name), Type(type), Kind(PinKind::Input), Value(0), Link(nullptr)
{
}

Blueprint::Link::Link(ax::NodeEditor::LinkId id, ax::NodeEditor::PinId startPinId, ax::NodeEditor::PinId endPinId):
    ID(id), StartPinID(startPinId), EndPinID(endPinId), Color(255, 255, 255)
{
}

ImColor Blueprint::GetIconColor(PinType type)
{
    switch (type)
    {
    default:
    case PinType::Flow:     return ImColor(255, 255, 255);
    case PinType::Bool:     return ImColor(220,  48,  48);
    case PinType::Array:      return ImColor( 68, 201, 156);
    case PinType::Float:    return ImColor(147, 226,  74);
    case PinType::Variable: return ImColor(147, 226,  74);
//    case PinType::String:   return ImColor(124,  21, 153);
//    case PinType::Object:   return ImColor( 51, 150, 215);
//    case PinType::Function: return ImColor(218,   0, 183);
//    case PinType::Delegate: return ImColor(255,  48,  48);
    }
}

void Blueprint::DrawPinIcon(const Pin &pin, bool connected, int alpha, ImVec2 IconsSize)
{
    ax::Widgets::IconType iconType;
    ImColor  color = GetIconColor(pin.Type);
    color.Value.w = alpha / 255.0f;
    switch (pin.Type)
    {
    case PinType::Flow:     iconType = ax::Widgets::IconType::Flow;   break;
    case PinType::Bool:     iconType = ax::Widgets::IconType::Circle; break;
    case PinType::Array:      iconType = ax::Widgets::IconType::Circle; break;
    case PinType::Float:    iconType = ax::Widgets::IconType::Circle; break;
    case PinType::Variable: iconType = ax::Widgets::IconType::Circle; break;
//    case PinType::String:   iconType = ax::Widgets::IconType::Circle; break;
//    case PinType::Object:   iconType = ax::Widgets::IconType::Circle; break;
//    case PinType::Function: iconType = ax::Widgets::IconType::Circle; break;
//    case PinType::Delegate: iconType = ax::Widgets::IconType::Square; break;
    default:
        return;
    }

    ax::Widgets::Icon(IconsSize, iconType, connected, color, ImColor(32, 32, 32, alpha));
}
