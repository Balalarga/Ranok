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
        if (!_bIsOpened)
            return;

        bool bCanClose = !(_windowFlags & ImGuiWindowFlags_NoTitleBar);
        if (ImGui::Begin(_title.c_str(), bCanClose ? &_bIsOpened : nullptr, _windowFlags))
        {
            for (auto& widget: _inners)
                widget->Render();
        }

        ImGui::End();

        if (!_bIsOpened)
            OnClose();
    }

    virtual void OnClose() {}

    void Open() { if (!_bIsOpened) _bIsOpened = true; }
    void Close() { if (_bIsOpened) _bIsOpened = false; }
    bool IsOpened() const { return _bIsOpened; }

    const std::string& Title() const { return _title; }

private:
    std::string _title;
    ImGuiWindowFlags _windowFlags;

    bool _bIsOpened = true;

    std::vector<std::unique_ptr<IWidget>> _inners;
};
