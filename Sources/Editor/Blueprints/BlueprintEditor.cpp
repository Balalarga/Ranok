#include "BlueprintEditor.h"

#include <Ranok/LanguageCore/Functions.h>
#include <Ranok/LanguageCore/Operations.h>

#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui_internal.h>
#include "builders.h"
#include "widgets.h"

#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <utility>
#include <fstream>

#include <Ranok/Utility/StringUtility.h>


namespace ed = ax::NodeEditor;
namespace util = ed::Utilities;

using ax::Widgets::IconType;

static inline ImRect ImGui_GetItemRect()
{
    return ImRect(ImGui::GetItemRectMin(), ImGui::GetItemRectMax());
}

static inline ImRect ImRect_Expanded(const ImRect& rect, float x, float y)
{
    auto result = rect;
    result.Min.x -= x;
    result.Min.y -= y;
    result.Max.x += x;
    result.Max.y += y;
    return result;
}

static const float s_TouchTime = 1.0f;
static std::map<ed::NodeId, float, BlueprintEditor::NodeIdLess> s_NodeTouchTime;
static int s_NextId = 1;
static inline int GetNextId() { return s_NextId++; }
static ed::LinkId GetNextLinkId()
{
    return ed::LinkId(GetNextId());
}
static void TouchNode(ed::NodeId id)
{
    s_NodeTouchTime[id] = s_TouchTime;
}
static float GetTouchProgress(ed::NodeId id)
{
    auto it = s_NodeTouchTime.find(id);
    if (it != s_NodeTouchTime.end() && it->second > 0.0f)
        return (s_TouchTime - it->second) / s_TouchTime;
    else
        return 0.0f;
}
static void UpdateTouch()
{
    const auto deltaTime = ImGui::GetIO().DeltaTime;
    for (auto& entry : s_NodeTouchTime)
    {
        if (entry.second > 0.0f)
            entry.second -= deltaTime;
    }
}

static bool CanCreateLink(BlueprintEditor::Pin* a, BlueprintEditor::Pin* b)
{
    if (!a || !b || a == b || a->Kind == b->Kind || a->Type != b->Type || a->Node == b->Node)
        return false;

    return true;
}

static void BuildNode(BlueprintEditor::Node* node)
{
    for (auto& input : node->Inputs)
    {
        input.Node = node;
        input.Kind = BlueprintEditor::PinKind::Input;
    }

    for (auto& output : node->Outputs)
    {
        output.Node = node;
        output.Kind = BlueprintEditor::PinKind::Output;
    }
}


BlueprintEditor::Node *BlueprintEditor::FindNode(ax::NodeEditor::NodeId id)
{
    for (auto& node : _nodes)
        if (node.ID == id)
            return &node;

    return nullptr;
}

BlueprintEditor::Link *BlueprintEditor::FindLink(ax::NodeEditor::LinkId id)
{
    for (auto& link : _links)
        if (link->ID == id)
            return link.get();

    return nullptr;
}

BlueprintEditor::Pin *BlueprintEditor::FindPin(const ax::NodeEditor::PinId& id)
{
    if (!id)
        return nullptr;

    for (auto& node : _nodes)
    {
        for (auto& pin : node.Inputs)
            if (pin.ID == id)
                return &pin;

        for (auto& pin : node.Outputs)
            if (pin.ID == id)
                return &pin;
    }

    return nullptr;
}

bool BlueprintEditor::IsPinLinked(ax::NodeEditor::PinId id)
{
    if (!id)
        return false;

    for (auto& link : _links)
        if (link->StartPin.ID == id || link->EndPin.ID == id)
            return true;

    return false;
}


ImVec2 BlueprintEditor::NodeIconsSize = ImVec2(20, 20);


BlueprintEditor::BlueprintEditor():
    _context(ed::CreateEditor()),
    _nodeHeaderBackground(ImageStorage::Get().Load("BlueprintBackground", "Blueprints/BlueprintBackground.png"))
{
//    ImGuiIO& io = ImGui::GetIO();
//    _textFont = io.Fonts->AddFontFromFileTTF(FileSystem::GetAssetPath("Fonts/Roboto-Regular.ttf").c_str(), 28);
    _textFont = ImGui::GetFont();

    ed::SetCurrentEditor(_context);
    AddDefaultNodes();
    ed::NavigateToContent();
    ed::SetCurrentEditor(nullptr);
}

BlueprintEditor::~BlueprintEditor()
{
    _links.clear();
    ed::DestroyEditor(_context);
}

ImColor GetIconColor(BlueprintEditor::PinType type)
{
    switch (type)
    {
        default:
        case BlueprintEditor::PinType::Flow:     return ImColor(255, 255, 255);
        case BlueprintEditor::PinType::Bool:     return ImColor(220,  48,  48);
        case BlueprintEditor::PinType::Object:    return ImColor( 68, 201, 156);
        case BlueprintEditor::PinType::Float:    return ImColor(147, 226,  74);
        case BlueprintEditor::PinType::Variable: return ImColor(147, 226,  74);
        case BlueprintEditor::PinType::Array:   return ImColor(124,  21, 153);
        case BlueprintEditor::PinType::String:   return ImColor( 51, 150, 215);
        case BlueprintEditor::PinType::Function: return ImColor(218,   0, 183);
        case BlueprintEditor::PinType::Delegate: return ImColor(255,  48,  48);
    }
};

void BlueprintEditor::DrawPinIcon(const Pin& pin, bool connected, int alpha)
{
    IconType iconType;
    ImColor  color = GetIconColor(pin.Type);
    color.Value.w = alpha / 255.0f;
    switch (pin.Type)
    {
        case PinType::Flow:     iconType = IconType::Flow;   break;
        case PinType::Bool:     iconType = IconType::Circle; break;
        case PinType::Array:    iconType = IconType::Square; break;
        case PinType::Float:    iconType = IconType::Circle; break;
        case PinType::Variable: iconType = IconType::Circle; break;
        case PinType::String:   iconType = IconType::Circle; break;
        case PinType::Object:   iconType = IconType::Circle; break;
        case PinType::Function: iconType = IconType::Circle; break;
        case PinType::Delegate: iconType = IconType::Square; break;
        default:
            return;
    }

    ax::Widgets::Icon(NodeIconsSize, iconType, connected, color, ImColor(32, 32, 32, alpha));
}

BlueprintEditor::Node* BlueprintEditor::ContextMenu()
{
    static ImGuiTextFilter Filter;
    /*
        static ImGuiTextFilter filter;
        ImGui::Text("Filter usage:\n"
                    "  \"\"         display all lines\n"
                    "  \"xxx\"      display lines containing \"xxx\"\n"
                    "  \"xxx,yyy\"  display lines containing \"xxx\" or \"yyy\"\n"
                    "  \"-xxx\"     hide lines containing \"xxx\"");
        filter.Draw();
        const char* lines[] = { "aaa1.c", "bbb1.c", "ccc1.c", "aaa2.cpp", "bbb2.cpp", "ccc2.cpp", "abc.h", "hello, world" };
        for (int i = 0; i < IM_ARRAYSIZE(lines); i++)
            if (filter.PassFilter(lines[i]))
                ImGui::BulletText("%s", lines[i]);
    */
    Filter.Draw("");

    if (Filter.IsActive())
        ImGui::SetNextTreeNodeOpen(true);
    if (ImGui::TreeNode("Binary operations"))
    {
        for (auto& op : Operations::GetBinaries())
        {
            if (Filter.PassFilter(op.Name().c_str()) && ImGui::MenuItem(op.Name().c_str()))
            {
                _nodes.push_back(Node(GetNextId(), op.Name().c_str()));
                _nodes.back().Type = NodeType::Simple;
                _nodes.back().Inputs.emplace_back(GetNextId(), "l", PinType::Float);
                _nodes.back().Inputs.emplace_back(GetNextId(), "r", PinType::Float);
                _nodes.back().Outputs.emplace_back(GetNextId(), "o", PinType::Float);
                ImGui::TreePop();
                return &_nodes.back();
            }
        }
        ImGui::TreePop();
    }

    if (Filter.IsActive())
        ImGui::SetNextTreeNodeOpen(true);
    if (ImGui::TreeNode("Unary operations"))
    {
        for (auto& op : Operations::GetUnaries())
        {
            if (Filter.PassFilter(op.Name().c_str()) && ImGui::MenuItem(op.Name().c_str()))
            {
                _nodes.push_back(Node(GetNextId(), op.Name().c_str()));
                _nodes.back().Type = NodeType::Simple;
                _nodes.back().Inputs.emplace_back(GetNextId(), "i", PinType::Float);
                _nodes.back().Outputs.emplace_back(GetNextId(), "o", PinType::Float);
                ImGui::TreePop();
                return &_nodes.back();
            }
        }
        ImGui::TreePop();
    }
    if (Filter.IsActive())
        ImGui::SetNextTreeNodeOpen(true);
    if (ImGui::TreeNode("Standart functions"))
    {
        for (auto& func : Functions::GetAll())
        {
            if (Filter.PassFilter(func.Name().c_str()) && ImGui::MenuItem(func.Name().c_str()))
            {
                _nodes.push_back(Node(GetNextId(), func.Name().c_str()));
                _nodes.back().Type = NodeType::Simple;
                _nodes.back().Descr = func.Desc();
                _nodes.back().Inputs.emplace_back(GetNextId(), "i", PinType::Float);
                _nodes.back().Outputs.emplace_back(GetNextId(), "o", PinType::Float);
                ImGui::TreePop();
                return &_nodes.back();
            }
        }
        ImGui::TreePop();
    }
    if (Filter.IsActive())
        ImGui::SetNextTreeNodeOpen(true);
    if (ImGui::TreeNode("Arrays"))
    {
        if (Filter.PassFilter("Get") && ImGui::MenuItem("Get"))
        {
            _nodes.push_back(Node(GetNextId(), "Get"));
            _nodes.back().Type = NodeType::Simple;
            _nodes.back().Descr = "Get array element";
            _nodes.back().Inputs.emplace_back(GetNextId(), "i", PinType::Array);
            _nodes.back().Outputs.emplace_back(GetNextId(), "0", PinType::Float);
            ImGui::TreePop();
            return &_nodes.back();
        }

        if (Filter.PassFilter("Make") && ImGui::MenuItem("Make"))
        {
            _nodes.push_back(Node(GetNextId(), "Make"));
            _nodes.back().Type = NodeType::Simple;
            _nodes.back().Descr = "Set array elements";
            _nodes.back().Inputs.emplace_back(GetNextId(), "0", PinType::Float);
            _nodes.back().Outputs.emplace_back(GetNextId(), "i", PinType::Array);
            ImGui::TreePop();
            return &_nodes.back();
        }
        ImGui::TreePop();
    }

    if (Filter.IsActive())
        ImGui::SetNextTreeNodeOpen(true);
    if (ImGui::TreeNode("Custom functions"))
    {
        for (auto& func : Functions::GetAllCustoms())
        {
            if (Filter.PassFilter(func.Name().c_str()) && ImGui::MenuItem(func.Name().c_str()))
            {
                _nodes.push_back(Node(GetNextId(), func.Name().c_str()));
                _nodes.back().Type = NodeType::Simple;
                _nodes.back().Descr = func.Info().Desc();
                for (auto& arg: func.Args())
                {
                    if (auto arr = dynamic_cast<ArrayExpression*>(arg->child.get()))
                    {
                        _nodes.back().Inputs.emplace_back(GetNextId(), arg->name.c_str(), PinType::Array);
                        _nodes.back().Inputs.back().Values.resize(arr->Values.size());
                    }
                    else
                    {
                        _nodes.back().Inputs.emplace_back(GetNextId(), arg->name.c_str(), PinType::Float);
                    }
                }

                if (func.Info().ReturnType().Type == LanguageType::DoubleArray)
                {
                    _nodes.back().Outputs.emplace_back(GetNextId(), "o", PinType::Array);
                    _nodes.back().Outputs.back().Values.resize(func.Info().ReturnType().Count);
                }
                else
                {
                    _nodes.back().Outputs.emplace_back(GetNextId(), "o", PinType::Float);
                }
                ImGui::TreePop();
                return &_nodes.back();
            }
        }
        ImGui::TreePop();
    }

    if (ImGui::MenuItem("Add variable"))
    {
        _nodes.push_back(Node(GetNextId(), "Name"));
        _nodes.back().Type = NodeType::Simple;
        _nodes.back().Descr = "User-defined variable";
        _nodes.back().Inputs.emplace_back(GetNextId(), "", PinType::Float);
        _nodes.back().Outputs.emplace_back(GetNextId(), "", PinType::Float);
        return &_nodes.back();
    }

    if (ImGui::MenuItem("Comment"))
    {
        _nodes.push_back(Node(GetNextId(), "Comment"));
        _nodes.back().Type = NodeType::Comment;
        _nodes.back().Name.resize(512);
        _nodes.back().Size = ImVec2(300, 200);
        return &_nodes.back();
    }

    return nullptr;
}

void BlueprintEditor::AddDefaultNodes()
{
    _nodes.push_back(Node(GetNextId(), "Space  "));
    _nodes.back().Type = NodeType::Simple;
    _nodes.back().Descr = "Space arguments";
    _nodes.back().CanDelete = false;
    _nodes.back().Outputs.emplace_back(GetNextId(), "s", PinType::Array);
    _nodes.back().Outputs.back().Values.resize(3);
    ed::SetNodePosition(_nodes.back().ID, ImVec2(0, 0));
    BuildNode(&_nodes.back());

    _nodes.push_back(Node(GetNextId(), "Result"));
    _nodes.back().Type = NodeType::Simple;
    _nodes.back().CanDelete = false;
    _nodes.back().Descr = "Result of function";
    _nodes.back().Inputs.emplace_back(GetNextId(), "", PinType::Float);
    ed::SetNodePosition(_nodes.back().ID, ImVec2(600, 0));
    BuildNode(&_nodes.back());
};

void BlueprintEditor::Render()
{
    static ed::NodeId contextNodeId = 0;
    static ed::LinkId contextLinkId = 0;
    static ed::PinId  contextPinId  = 0;
    static bool createNewNode  = false;
    static Pin* newNodeLinkPin = nullptr;
    static Pin* newLinkPin     = nullptr;

    ed::SetCurrentEditor(_context);
    if (ImGui::IsKeyDown(ImGui::GetKeyIndex(ImGuiKey_Space)))
        ed::NavigateToContent();

    static enum class NodeCreationState
    {
        None, Opened, Opening
    } nodeCreationState = NodeCreationState::None;

    ed::Begin("Node editor");
    {
        auto cursorTopLeft = ImGui::GetCursorScreenPos();

        util::BlueprintNodeBuilder builder((ImTextureID)_nodeHeaderBackground.id, _nodeHeaderBackground.width, _nodeHeaderBackground.height);

        for (auto& node : _nodes)
        {
            if (node.Type != NodeType::Blueprint && node.Type != NodeType::Simple)
                continue;

            const auto isSimple = node.Type == NodeType::Simple;

            bool hasOutputDelegates = false;
            for (auto& output : node.Outputs)
                if (output.Type == PinType::Delegate)
                    hasOutputDelegates = true;

            builder.Begin(node.ID);
                if (!isSimple)
                {
                    builder.Header(node.Color);
                        ImGui::Spring(0);
                        ImGui::PushFont(_textFont);
                        ImGui::TextUnformatted(node.Name.c_str());
                        ImGui::PopFont();
                        ImGui::Spring(1);
                        ImGui::Dummy(ImVec2(0, 28));
                        if (hasOutputDelegates)
                        {
                            ImGui::BeginVertical("delegates", ImVec2(0, 28));
                            ImGui::Spring(1, 0);
                            for (auto& output : node.Outputs)
                            {
                                if (output.Type != PinType::Delegate)
                                    continue;

                                auto alpha = ImGui::GetStyle().Alpha;
                                if (newLinkPin && !CanCreateLink(newLinkPin, &output) && &output != newLinkPin)
                                    alpha = alpha * (48.0f / 255.0f);

                                ed::BeginPin(output.ID, ed::PinKind::Output);
                                ed::PinPivotAlignment(ImVec2(1.0f, 0.5f));
                                ed::PinPivotSize(ImVec2(0, 0));
                                ImGui::BeginHorizontal(output.ID.AsPointer());
                                ImGui::PushStyleVar(ImGuiStyleVar_Alpha, alpha);
                                if (!output.Name.empty())
                                {
                                    ImGui::TextUnformatted(output.Name.c_str());
                                    ImGui::Spring(0);
                                }
                                DrawPinIcon(output, IsPinLinked(output.ID), (int)(alpha * 255));
                                ImGui::Spring(0, ImGui::GetStyle().ItemSpacing.x / 2);
                                ImGui::EndHorizontal();
                                ImGui::PopStyleVar();
                                ed::EndPin();

                                //DrawItemRect(ImColor(255, 0, 0));
                            }
                            ImGui::Spring(1, 0);
                            ImGui::EndVertical();
                            ImGui::Spring(0, ImGui::GetStyle().ItemSpacing.x / 2);
                        }
                        else
                            ImGui::Spring(0);
                    builder.EndHeader();
                }

                for (auto& input : node.Inputs)
                {
                    auto alpha = ImGui::GetStyle().Alpha;
                    if (newLinkPin && !CanCreateLink(newLinkPin, &input) && &input != newLinkPin)
                        alpha = alpha * (48.0f / 255.0f);

                    builder.Input(input.ID);
                    ImGui::PushStyleVar(ImGuiStyleVar_Alpha, alpha);
                    DrawPinIcon(input, IsPinLinked(input.ID), (int)(alpha * 255));
                    ImGui::Spring(0);
                    if (input.Linker.empty())
                    {
                        ImGui::SetNextItemWidth(ImGui::GetFontSize() * 6);
                        if (input.Type != PinType::Array)
                        {
                            ImGui::InputDouble("", &input.Values[0], 0, 0, "%.3f");
                        }
                    }
                    if (!input.Name.empty())
                    {
                        ImGui::TextUnformatted(input.Name.c_str());
                        ImGui::Spring(0);
                    }
                    if (input.Type == PinType::Bool)
                    {
                         ImGui::Button("Hello");
                         ImGui::Spring(0);
                    }
                    ImGui::PopStyleVar();
                    builder.EndInput();
                }

                if (isSimple)
                {
                    builder.Middle();

                    ImGui::Spring(1, 0);
                    ImGui::PushFont(_textFont);
                    ImGui::TextUnformatted(node.Name.c_str());
                    ImGui::PopFont();
                    ImGui::Spring(1, 0);
                }

                for (auto& output : node.Outputs)
                {
                    if (!isSimple && output.Type == PinType::Delegate)
                        continue;

                    auto alpha = ImGui::GetStyle().Alpha;
                    if (newLinkPin && !CanCreateLink(newLinkPin, &output) && &output != newLinkPin)
                        alpha = alpha * (48.0f / 255.0f);

                    ImGui::PushStyleVar(ImGuiStyleVar_Alpha, alpha);
                    builder.Output(output.ID);
                    if (output.Type == PinType::String)
                    {
                        static char buffer[128] = "Edit Me\nMultiline!";
                        static bool wasActive = false;

                        ImGui::PushItemWidth(100.0f);
                        ImGui::InputText("##edit", buffer, 127);
                        ImGui::PopItemWidth();
                        if (ImGui::IsItemActive() && !wasActive)
                        {
                            ed::EnableShortcuts(false);
                            wasActive = true;
                        }
                        else if (!ImGui::IsItemActive() && wasActive)
                        {
                            ed::EnableShortcuts(true);
                            wasActive = false;
                        }
                        ImGui::Spring(0);
                    }
                    if (!output.Name.empty())
                    {
                        ImGui::Spring(0);
                        ImGui::TextUnformatted(output.Name.c_str());
                    }
                    ImGui::Spring(0);
                    DrawPinIcon(output, IsPinLinked(output.ID), (int)(alpha * 255));
                    ImGui::PopStyleVar();
                    builder.EndOutput();
                }

            builder.End();
        }

        // Comment
        for (auto& node : _nodes)
        {
            if (node.Type != NodeType::Comment)
                continue;

            const float commentAlpha = 0.75f;

            ImGui::PushStyleVar(ImGuiStyleVar_Alpha, commentAlpha);
            ed::PushStyleColor(ed::StyleColor_NodeBg, ImColor(255, 255, 255, 64));
            ed::PushStyleColor(ed::StyleColor_NodeBorder, ImColor(255, 255, 255, 64));
            ed::BeginNode(node.ID);
            ImGui::PushID(node.ID.AsPointer());
            ImGui::BeginVertical("content");
            ImGui::BeginHorizontal("horizontal");
            ImGui::Spring(1);
            ImGui::PushFont(_textFont);
            ImGui::InputText("", &node.Name[0], node.Name.size());
            ImGui::PopFont();
            ImGui::Spring(1);
            ImGui::EndHorizontal();
            ed::Group(node.Size);
            ImGui::EndVertical();
            ImGui::PopID();
            ed::EndNode();
            ed::PopStyleColor(2);
            ImGui::PopStyleVar();

            if (ed::BeginGroupHint(node.ID))
            {
                //auto alpha   = static_cast<int>(commentAlpha * ImGui::GetStyle().Alpha * 255);
                auto bgAlpha = static_cast<int>(ImGui::GetStyle().Alpha * 255);

                //ImGui::PushStyleVar(ImGuiStyleVar_Alpha, commentAlpha * ImGui::GetStyle().Alpha);

                auto min = ed::GetGroupMin();
                //auto max = ed::GetGroupMax();

                ImGui::SetCursorScreenPos(min - ImVec2(-8, ImGui::GetTextLineHeightWithSpacing() + 4));
                ImGui::BeginGroup();
                ImGui::PushFont(_textFont);
                ImGui::TextUnformatted(node.Name.c_str());
                ImGui::PopFont();
                ImGui::EndGroup();

                auto drawList = ed::GetHintBackgroundDrawList();

                auto hintBounds      = ImGui_GetItemRect();
                auto hintFrameBounds = ImRect_Expanded(hintBounds, 8, 4);

                drawList->AddRectFilled(
                    hintFrameBounds.GetTL(),
                    hintFrameBounds.GetBR(),
                    IM_COL32(255, 255, 255, 64 * bgAlpha / 255), 4.0f);

                drawList->AddRect(
                    hintFrameBounds.GetTL(),
                    hintFrameBounds.GetBR(),
                    IM_COL32(255, 255, 255, 128 * bgAlpha / 255), 4.0f);

                //ImGui::PopStyleVar();
            }
            ed::EndGroupHint();
        }

        for (auto& link : _links)
            ed::Link(link->ID, link->StartPin.ID, link->EndPin.ID, link->Color, 2.0f);

        if (!createNewNode)
        {
            if (ed::BeginCreate(ImColor(255, 255, 255), 2.0f))
            {
                auto showLabel = [](const char* label, ImColor color)
                {
                    ImGui::SetCursorPosY(ImGui::GetCursorPosY() - ImGui::GetTextLineHeight());
                    auto size = ImGui::CalcTextSize(label);

                    auto padding = ImGui::GetStyle().FramePadding;
                    auto spacing = ImGui::GetStyle().ItemSpacing;

                    ImGui::SetCursorPos(ImGui::GetCursorPos() + ImVec2(spacing.x, -spacing.y));

                    auto rectMin = ImGui::GetCursorScreenPos() - padding;
                    auto rectMax = ImGui::GetCursorScreenPos() + size + padding;

                    auto drawList = ImGui::GetWindowDrawList();
                    drawList->AddRectFilled(rectMin, rectMax, color, size.y * 0.15f);
                    ImGui::TextUnformatted(label);
                };

                ed::PinId startPinId = 0, endPinId = 0;
                if (ed::QueryNewLink(&startPinId, &endPinId))
                {
                    auto startPin = FindPin(startPinId);
                    auto endPin   = FindPin(endPinId);

                    newLinkPin = startPin ? startPin : endPin;

                    if (startPin->Kind == PinKind::Input)
                    {
                        std::swap(startPin, endPin);
                        std::swap(startPinId, endPinId);
                    }

                    if (startPin && endPin)
                    {
                        if (endPin == startPin)
                        {
                            ed::RejectNewItem(ImColor(255, 0, 0), 2.0f);
                        }
                        else if (endPin->Kind == startPin->Kind)
                        {
                            showLabel("x Incompatible Pin Kind", ImColor(45, 32, 32, 180));
                            ed::RejectNewItem(ImColor(255, 0, 0), 2.0f);
                        }
                        else if (endPin->Node == startPin->Node)
                        {
                            showLabel("x Cannot connect to self", ImColor(45, 32, 32, 180));
                            ed::RejectNewItem(ImColor(255, 0, 0), 1.0f);
                        }
                        else if (endPin->Type != startPin->Type)
                        {
                            showLabel("x Incompatible Pin Type", ImColor(45, 32, 32, 180));
                            ed::RejectNewItem(ImColor(255, 128, 128), 1.0f);
                        }
                        else
                        {
                            showLabel("+ Create Link", ImColor(32, 45, 32, 180));
                            if (ed::AcceptNewItem(ImColor(128, 255, 128), 4.0f))
                            {
                                if (!endPin->Linker.empty())
                                {
                                    auto it = std::find_if(_links.begin(), _links.end(),[&endPin](const std::unique_ptr<Link>& link){ return endPin->Linker[0] == link.get(); });
                                    if (it != _links.end())
                                        _links.erase(it);
                                }

                                _links.emplace_back(std::make_unique<Link>(GetNextId(), *startPin, *endPin));
                                if (endPin->Type == PinType::Array && startPin->Node->Name == "Make")
                                {
                                    for (auto& i : startPin->Node->Inputs)
                                    {
                                        for (auto& l: i.Linker)
                                        {
                                            auto id = std::find_if(_links.begin(), _links.end(), [l](auto& link) { return link->ID == l->ID; });
                                            if (id != _links.end())
                                                _links.erase(id);
                                        }
                                    }
                                    startPin->Node->Inputs.clear();
                                    for (int i = 0; i < endPin->Values.size(); ++i)
                                        startPin->Node->Inputs.emplace_back(GetNextId(), std::to_string(i).c_str(), PinType::Float);
                                    BuildNode(startPin->Node);
                                }
                                else if (endPin->Type == PinType::Array && endPin->Node->Name == "Get")
                                {
                                    for (auto& i : endPin->Node->Outputs)
                                    {
                                        for (auto& l: i.Linker)
                                        {
                                            auto id = std::find_if(_links.begin(), _links.end(), [l](auto& link) { return link->ID == l->ID; });
                                            if (id != _links.end())
                                                _links.erase(id);
                                        }
                                    }
                                    endPin->Node->Outputs.clear();
                                    for (int i = 0; i < startPin->Values.size(); ++i)
                                        endPin->Node->Outputs.emplace_back(GetNextId(), std::to_string(i).c_str(), PinType::Float);
                                    BuildNode(endPin->Node);
                                }
                                _links.back()->Color = GetIconColor(startPin->Type);
                            }
                        }
                    }
                }

                ed::PinId pinId = 0;
                if (ed::QueryNewNode(&pinId))
                {
                    newLinkPin = FindPin(pinId);
                    if (newLinkPin)
                        showLabel("+ Create Node", ImColor(32, 45, 32, 180));

                    if (ed::AcceptNewItem())
                    {
                        createNewNode  = true;
                        newNodeLinkPin = FindPin(pinId);
                        newLinkPin = nullptr;
                        ed::Suspend();
                        ImGui::OpenPopup("Create New Node");
                        nodeCreationState = NodeCreationState::Opening;
                        ed::Resume();
                    }
                }
            }
            else
                newLinkPin = nullptr;

            ed::EndCreate();

            if (ed::BeginDelete())
            {
                ed::LinkId linkId = 0;
                while (ed::QueryDeletedLink(&linkId))
                {
                    if (ed::AcceptDeletedItem())
                    {
                        auto id = std::find_if(_links.begin(), _links.end(), [linkId](auto& link) { return link->ID == linkId; });
                        if (id != _links.end())
                        {
                            _links.erase(id);
                        }
                    }
                }

                ed::NodeId nodeId = 0;
                while (ed::QueryDeletedNode(&nodeId))
                {
                    if (ed::AcceptDeletedItem())
                    {
                        auto id = std::find_if(_nodes.begin(), _nodes.end(), [nodeId](auto& node) { return node.ID == nodeId; });
                        if (id != _nodes.end() && id->CanDelete)
                            _nodes.erase(id);
                    }
                }
            }
            ed::EndDelete();
        }

        ImGui::SetCursorScreenPos(cursorTopLeft);
    }

# if 1
    auto openPopupPosition = ImGui::GetMousePos();
    ed::Suspend();
    if (ed::ShowNodeContextMenu(&contextNodeId))
        ImGui::OpenPopup("Node Context Menu");
    else if (ed::ShowPinContextMenu(&contextPinId))
        ImGui::OpenPopup("Pin Context Menu");
    else if (ed::ShowLinkContextMenu(&contextLinkId))
        ImGui::OpenPopup("Link Context Menu");
    else if (ed::ShowBackgroundContextMenu())
    {
        ImGui::OpenPopup("Create New Node");
        nodeCreationState = NodeCreationState::Opening;
        newNodeLinkPin = nullptr;
    }
    ed::Resume();

    ed::Suspend();
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));
    if (ImGui::BeginPopup("Node Context Menu"))
    {
        auto node = FindNode(contextNodeId);

        ImGui::TextUnformatted("Node Context Menu");
        ImGui::Separator();
        if (node)
        {
            ImGui::Text("Description: %s", node->Descr.c_str());
            ImGui::Text("Inputs: %d", (int)node->Inputs.size());
            ImGui::Text("Outputs: %d", (int)node->Outputs.size());
        }
        else
            ImGui::Text("Unknown node: %p", contextNodeId.AsPointer());
        ImGui::Separator();
        if (ImGui::MenuItem("Delete"))
            ed::DeleteNode(contextNodeId);
        ImGui::EndPopup();
    }

    if (ImGui::BeginPopup("Pin Context Menu"))
    {
        auto pin = FindPin(contextPinId);

        ImGui::TextUnformatted("Pin Context Menu");
        ImGui::Separator();
        if (pin)
        {
            if (pin->Node)
                ImGui::Text("Node: %s", pin->Node->Name.c_str());
            else
                ImGui::Text("Node: %s", "<none>");
        }
        else
            ImGui::Text("Unknown pin: %p", contextPinId.AsPointer());

        ImGui::EndPopup();
    }

    if (ImGui::BeginPopup("Link Context Menu"))
    {
        auto link = FindLink(contextLinkId);

        ImGui::TextUnformatted("Link Context Menu");
        ImGui::Separator();
        if (link)
        {
            ImGui::Text("From: %s(%s)", link->StartPin.Node->Name.c_str(), link->StartPin.Name.c_str());
            ImGui::Text("To:   %s(%s)", link->EndPin.Node->Name.c_str(), link->EndPin.Name.c_str());
        }
        else
            ImGui::Text("Unknown link: %p", contextLinkId.AsPointer());
        ImGui::Separator();
        if (ImGui::MenuItem("Delete"))
            ed::DeleteLink(contextLinkId);
        ImGui::EndPopup();
    }

    ImGui::SetNextWindowSizeConstraints({0, 0}, {ImGui::GetWindowWidth()/3, ImGui::GetWindowHeight()/3});
    if (ImGui::BeginPopup("Create New Node"))
    {
        static auto newNodePostion = openPopupPosition;
        if (nodeCreationState == NodeCreationState::Opening)
        {
            newNodePostion = openPopupPosition;
            nodeCreationState = NodeCreationState::Opened;
        }
        Node* node = ContextMenu();

        if (node)
        {
            for (auto& i : _nodes)
                BuildNode(&i);

            createNewNode = false;

            ed::SetNodePosition(node->ID, newNodePostion);

            if (auto startPin = newNodeLinkPin)
            {
                auto& pins = startPin->Kind == PinKind::Input ? node->Outputs : node->Inputs;

                for (auto& pin : pins)
                {
                    if (CanCreateLink(startPin, &pin))
                    {
                        auto endPin = &pin;
                        if (startPin->Kind == PinKind::Input)
                            std::swap(startPin, endPin);

                        if (!endPin->Linker.empty())
                        {
                            auto it = std::find_if(_links.begin(), _links.end(), [&endPin](const std::unique_ptr<Link>& link){ return endPin->Linker[0] == link.get(); });
                            if (it != _links.end())
                                _links.erase(it);
                        }

                        _links.emplace_back(std::make_unique<Link>(GetNextId(), *startPin, *endPin));
                        if (endPin->Type == PinType::Array && startPin->Node->Name == "Make")
                        {
                            for (auto& i : startPin->Node->Inputs)
                            {
                                for (auto& l: i.Linker)
                                {
                                    auto id = std::find_if(_links.begin(), _links.end(), [l](auto& link) { return link->ID == l->ID; });
                                    if (id != _links.end())
                                        _links.erase(id);
                                }
                            }
                            startPin->Node->Inputs.clear();
                            for (int i = 0; i < endPin->Values.size(); ++i)
                                startPin->Node->Inputs.emplace_back(GetNextId(), std::to_string(i).c_str(), PinType::Float);
                            BuildNode(startPin->Node);
                        }
                        else if (endPin->Type == PinType::Array && endPin->Node->Name == "Get")
                        {
                            for (auto& i : endPin->Node->Outputs)
                            {
                                for (auto& l: i.Linker)
                                {
                                    auto id = std::find_if(_links.begin(), _links.end(), [l](auto& link) { return link->ID == l->ID; });
                                    if (id != _links.end())
                                        _links.erase(id);
                                }
                            }
                            endPin->Node->Outputs.clear();
                            for (int i = 0; i < startPin->Values.size(); ++i)
                                endPin->Node->Outputs.emplace_back(GetNextId(), std::to_string(i).c_str(), PinType::Float);
                            BuildNode(endPin->Node);
                        }
                        _links.back()->Color = GetIconColor(startPin->Type);

                        break;
                    }
                }
            }
        }

        ImGui::EndPopup();
    }
    else
        createNewNode = false;
    ImGui::PopStyleVar();
    ed::Resume();
# endif

    ed::End();

    ed::SetCurrentEditor(nullptr);
}

void BlueprintEditor::Save(const std::string &filepath)
{
    std::ofstream file(filepath);
    if (!file)
        return;

    auto WriteString = [](std::ofstream& file, const std::string& string)
    {
        size_t stringSize = string.size();
        file.write((char*)&stringSize, sizeof(stringSize));
        file.write((char*)&string[0], sizeof(char)*stringSize);
    };

    auto nodesSize = _nodes.size();
    file.write((char*)&nodesSize, sizeof(nodesSize));
    for (auto& i: _nodes)
    {
        file.write((char*)&i.ID, sizeof(i.ID));
        WriteString(file, i.Name);
        file.write((char*)&i.Color, sizeof(i.Color));

        auto size = i.Inputs.size();
        file.write((char*)&size, sizeof(size));
        for (auto& pin: i.Inputs)
        {
            file.write((char*)&pin.ID, sizeof(pin.ID));
            WriteString(file, pin.Name);
            file.write((char*)&pin.Type, sizeof(pin.Type));
//            file.write((char*)&pin.Value, sizeof(pin.Value));
        }

        size = i.Outputs.size();
        file.write((char*)&size, sizeof(size));
        for (auto& pin: i.Outputs)
        {
            file.write((char*)&pin.ID, sizeof(pin.ID));
            WriteString(file, pin.Name);
            file.write((char*)&pin.Type, sizeof(pin.Type));
//            file.write((char*)&pin.Value, sizeof(pin.Value));
        }

        file.write((char*)&i.Type, sizeof(i.Type));
        file.write((char*)&i.Size, sizeof(i.Size));
        WriteString(file, i.Descr);
        file.write((char*)&i.CanDelete, sizeof(i.CanDelete));
        WriteString(file, i.State);
        WriteString(file, i.SavedState);
    }

    auto linksSize = _links.size();
    file.write((char*)&linksSize, sizeof(linksSize));
    for (auto& i: _links)
    {
        file.write((char*)&i->ID, sizeof(i->ID));
        file.write((char*)&i->StartPin.ID, sizeof(i->StartPin.ID));
        file.write((char*)&i->EndPin.ID, sizeof(i->EndPin.ID));
    }

    file.close();
}

void BlueprintEditor::Open(const std::string &filepath)
{
    std::ifstream file(filepath);
    if (!file)
        return;

    _nodes.clear();
    _links.clear();

    auto nodesSize = _nodes.size();
    file.read((char*)&nodesSize, sizeof(nodesSize));


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


    ax::NodeEditor::PinId PinID;
    std::string PinName;
    size_t PinNameSize;
    PinType     PinType;
    double      PinValue;
    auto ReadString = [](std::ifstream& file) -> std::string
    {
        size_t stringSize;
        file.read((char*)&stringSize, sizeof(stringSize));
        std::string string(stringSize, ' ');
        file.read((char*)&string[0], sizeof(char)*stringSize);
        return string;
    };

    for (size_t i = 0; i < nodesSize; ++i)
    {
         file.read((char*)&ID, sizeof(ID));
         Name = ReadString(file);
         file.read((char*)&Color, sizeof(Color));

         auto size = Inputs.size();
         file.read((char*)&size, sizeof(size));
         for (size_t i = 0; i < size; ++i)
         {
             file.read((char*)&PinID, sizeof(PinID));
             PinName = ReadString(file);
             file.read((char*)&PinType, sizeof(PinType));
             file.read((char*)&PinValue, sizeof(PinValue));
             Inputs.emplace_back(PinID, PinName.c_str(), PinType);
//             Inputs.back().Value = PinValue;
         }

         file.read((char*)&size, sizeof(size));
         Outputs.resize(size);
         for (size_t i = 0; i < size; ++i)
         {
             file.read((char*)&PinID, sizeof(PinID));
             PinName = ReadString(file);
             file.read((char*)&PinType, sizeof(PinType));
             file.read((char*)&PinValue, sizeof(PinValue));
             Outputs.emplace_back(PinID, PinName.c_str(), PinType);
//             Outputs.back().Value = PinValue;
         }

         file.read((char*)&Type, sizeof(Type));
         file.read((char*)&Size, sizeof(Size));
         Descr = ReadString(file);
         file.read((char*)&CanDelete, sizeof(CanDelete));
         State = ReadString(file);
         SavedState = ReadString(file);

         _nodes.push_back(Node(ID, Name.c_str(), Color));
         _nodes.back().Inputs = Inputs;
         _nodes.back().Outputs = Outputs;
    }

    auto linksSize = _links.size();
    file.read((char*)&linksSize, sizeof(linksSize));
    ax::NodeEditor::LinkId linkId;
    ax::NodeEditor::PinId startPinId;
    ax::NodeEditor::PinId endPinId;
    for (size_t i = 0; i < linksSize; ++i)
    {
        file.read((char*)&linkId, sizeof(linkId));
        file.read((char*)&startPinId, sizeof(startPinId));
        file.read((char*)&endPinId, sizeof(endPinId));

        auto startPin = FindPin(startPinId);
        auto endPin = FindPin(endPinId);

        _links.push_back(std::make_unique<Link>(linkId, *startPin, *endPin));
    }

    file.close();

    for (auto& i: _nodes)
        BuildNode(&i);
}

spExpression BlueprintEditor::Iterate(Program& program, Pin& pin)
{
    auto CheckPin = [&program, this](Pin& pin) -> spExpression
    {
        return pin.Linker.empty() ? program.Table().CreateConstant(pin.Values[0]) :
                             Iterate(program, pin.Linker[0]->StartPin);
    };

    if (pin.Linker.empty())
        return program.Table().CreateConstant(pin.Values[0]);

    if (pin.Node->Name == "Get")
    {
        auto sharedVar = std::dynamic_pointer_cast<VariableExpression>(CheckPin(pin.Node->Inputs[0]));
        if (auto var = dynamic_cast<VariableExpression*>(sharedVar.get()))
        {
            if (auto arr = dynamic_cast<ArrayExpression*>(var->child.get()))
                return std::make_shared<ArrayGetterExpression>(sharedVar, std::stoi(pin.Name));
            return nullptr;
        }
        else
        {
            return nullptr;
        }
    }

    if (pin.Node->Name == "Make")
    {
        std::vector<spExpression> values;
        for (auto &i: pin.Node->Inputs)
            values.push_back(CheckPin(i));
        std::string randomName;
        while (true)
        {
            randomName = StringUtility::GetRandomString(10 + rand() % 10);
            if (!program.Table().FindArgument(randomName) && !program.Table().FindVariable(randomName))
                break;
        }

        return program.Table().CreateVariable(randomName, std::make_shared<ArrayExpression>(values));
    }


    Node* node = pin.Node;

    auto expr = program.Table().FindArgument(pin.Name);
    if (expr)
        return expr;

    auto unOp = Operations::UnaryFromString(node->Name);
    if (unOp && node->Inputs.size() == 1)
    {
        return std::make_shared<UnaryOperation>(*unOp,
                                                CheckPin(node->Inputs[0]));
    }

    auto binOp = Operations::BinaryFromString(node->Name);
    if (binOp && node->Inputs.size() == 2)
    {
        return std::make_shared<BinaryOperation>(*binOp,
                                                 CheckPin(node->Inputs[0]),
                                                 CheckPin(node->Inputs[1]));
    }

    if (auto func = Functions::Find(node->Name))
    {
        std::vector<spExpression> args(node->Inputs.size());
        for (size_t i = 0; i < args.size(); ++i)
            args[i] = CheckPin(node->Inputs[i]);
        return std::make_shared<FunctionExpression>(*func, args);
    }

    if (auto func = Functions::FindCustom(node->Name))
    {
        if ( func->Info().ReturnType().Type == LanguageType::Double)
        {
            std::vector<spExpression> args(node->Inputs.size());
            for (size_t i = 0; i < args.size(); ++i)
                args[i] = CheckPin(node->Inputs[i]);
            return std::make_shared<CustomFunctionExpression>(func->Info(), func->Root(), args);
        }
        else
        {
            std::vector<spExpression> args(node->Inputs.size());
            for (size_t i = 0; i < args.size(); ++i)
                args[i] = CheckPin(node->Inputs[i]);

            std::string randomName;
            while (true)
            {
                randomName = StringUtility::GetRandomString(10 + rand() % 10);
                if (!program.Table().FindArgument(randomName) && !program.Table().FindVariable(randomName))
                    break;
            }
            auto var = program.Table().CreateVariable(randomName, std::make_shared<CustomFunctionExpression>(func->Info(), func->Root(), args));
            return var;
        }
    }

    return nullptr;
}

Program BlueprintEditor::GetProgram()
{
    Program program;

    auto BeginNode = _nodes[0];
    auto ResultNode = _nodes[1];

    std::vector<Range> ranges(BeginNode.Outputs[0].Values.size(), {-1, 1});
    program.Table().CreateArgument(BeginNode.Outputs[0].Name, ranges);

    if (!ResultNode.Inputs[0].Linker.empty())
        program.Init(Iterate(program, ResultNode.Inputs[0].Linker[0]->StartPin));

    return program;
}
