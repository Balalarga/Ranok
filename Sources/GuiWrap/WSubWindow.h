#pragma once

#include <string>
#include <vector>

#include "IWidget.h"
#include "Utils/MemoryUtils.h"

class WSubWindow: public IWidget
{
public:
    WSubWindow(const std::string& title, ImGuiWindowFlags flags = ImGuiWindowFlags_None):
        _title(title), _windowFlags(flags) {}

    DECLARE_VECTOR_UNIQUE_ADDER(Add, _inners)

    void Render() override
    {
        if (ImGui::BeginChild(_title.c_str(), ImVec2(), _bBorder, _windowFlags))
        {
            for (std::unique_ptr<IWidget>& widget : _inners)
                widget->Render();
        }
        ImGui::EndChild();
    }
    
    const std::string& Title() const { return _title; }

protected:
    bool _bBorder = true;
    
private:
    std::string _title;
    ImGuiWindowFlags _windowFlags;
    
    std::vector<std::unique_ptr<IWidget>> _inners;
};
