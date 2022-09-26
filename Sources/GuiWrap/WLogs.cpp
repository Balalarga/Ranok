#include "WLogs.h"

namespace Ranok
{
WLogs::WLogs():
    WWindow("Logs")
{
}

void WLogs::AddLog(const std::string& text)
{
    _textBuffer.append(text.c_str());
    _textBuffer.append("\n");
    _bScrollToBottom = true;
}

void WLogs::Render()
{
    ImGui::Begin("Log", &bIsOpened);
    if (ImGui::Button("Clear"))
        Clear();
    ImGui::SameLine();
    bool copy = ImGui::Button("Copy");
    ImGui::Separator();
    ImGui::BeginChild("scrolling");
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0,1));
    if (copy) ImGui::LogToClipboard();
    ImGui::TextUnformatted(_textBuffer.begin());
    if (_bScrollToBottom)
        ImGui::SetScrollHereY(1.0f);
    
    _bScrollToBottom = false;
    ImGui::PopStyleVar();
    ImGui::EndChild();
    ImGui::End();
}

void WLogs::Clear()
{
    _textBuffer.clear();
}
}
