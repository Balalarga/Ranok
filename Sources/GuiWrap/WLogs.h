#pragma once

#include <GuiWrap/WWindow.h>

namespace Ranok
{
class WLogs: public WWindow
{
public:
    WLogs();
    
    void AddLog(const std::string& text);
    void Clear();
    
    void Render() override;
    
    
private:
    ImGuiTextBuffer _textBuffer;
    bool _bScrollToBottom = true;
};
}
