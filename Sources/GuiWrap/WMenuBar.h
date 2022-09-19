#pragma once

#include "IWidget.h"
#include <functional>
#include <string>
#include <vector>
#include <memory>
#include <iostream>
#include "Utils/MemoryUtils.h"

namespace Ranok
{
class WMenuItem: public IWidget
{
public:
    WMenuItem(const std::string& text, std::function<void()>&& onPress = {}):
        _text(text),
        _onPress(onPress)
    {
    }

    void Render()
    {
        if (ImGui::MenuItem(_text.c_str()) && _onPress)
            _onPress();
    }

private:
    std::string _text;
    std::function<void()> _onPress;
};

class WMenuCheckItem: public IWidget
{
public:
    WMenuCheckItem(const std::string& text, std::function<void(bool)>&& onPress = {}):
        _text(text),
        _onPress(onPress)
    {
    }

    void Render()
    {
        if (ImGui::MenuItem(_text.c_str(), nullptr, &_bChecked) && _onPress)
            _onPress(_bChecked);
    }

    void SetChecked(bool state, bool force = false)
    {
        _bChecked = state;
        if (!force)
            _onPress(state);
    }

private:
    std::string _text;
    bool _bChecked = false;
    std::function<void(bool)> _onPress;
};

class WMenu: public IWidget
{
public:
    WMenu(const std::string& title):
        _title(title) {}

    DECLARE_VECTOR_UNIQUE_ADDER(Add, _items)

    void Render()
    {
        if (ImGui::BeginMenu(_title.c_str()))
        {
            for (auto& item: _items)
                item->Render();

            ImGui::EndMenu();
        }
    }

private:
    std::string _title;
    std::vector<std::unique_ptr<IWidget>> _items;
};

/// Don't forget to add ImGuiWindowFlags_MenuBar to target window
class WMenuBar: public IWidget
{
public:
    DECLARE_VECTOR_UNIQUE_ADDER(Add, _menus)

    void Render()
    {
        if (ImGui::BeginMenuBar())
        {
            for (auto& menu: _menus)
                menu->Render();

            ImGui::EndMenuBar();
        }
    }

private:
    std::vector<std::unique_ptr<WMenu>> _menus;
};
}
