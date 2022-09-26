#pragma once

#include <string>
#include <vector>
#include <memory>
#include "IWidget.h"
#include "Utils/MemoryUtils.h"

class WWindow: public IWidget
{
public:
    WWindow(const std::string& title, ImGuiWindowFlags flags = ImGuiWindowFlags_None):
        _title(title), _windowFlags(flags) {}

    DECLARE_VECTOR_UNIQUE_ADDER(Add, _inners)

    void Render() override
    {
        if (!bIsOpened)
            return;

        bool bCanClose = !(_windowFlags & ImGuiWindowFlags_NoTitleBar);
        if (ImGui::Begin(_title.c_str(), bCanClose ? &bIsOpened : nullptr, _windowFlags))
        {
            for (std::unique_ptr<IWidget>& widget : _inners)
                widget->Render();
        }

        ImGui::End();

        if (!bIsOpened)
            OnClose();
    }

    virtual void OnClose() {}

    void Open() { if (!bIsOpened) bIsOpened = true; }
    void Close() { if (bIsOpened) bIsOpened = false; }
    bool IsOpened() const { return bIsOpened; }

    const std::string& Title() const { return _title; }

protected:
    bool bIsOpened = true;
    
private:
    std::string _title;
    ImGuiWindowFlags _windowFlags;
    
    std::vector<std::unique_ptr<IWidget>> _inners;
};
