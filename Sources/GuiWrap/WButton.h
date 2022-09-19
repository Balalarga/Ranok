#pragma once

#include <functional>
#include <string>
#include "IWidget.h"

class WButton: public IWidget
{
public:
    WButton(const std::string& text, const std::function<void()>& onPress = {}):
        _text(text), _onPress(onPress) {}
    
    void Render() override
    {
        if (ImGui::Button(_text.c_str()) && _onPress)
            _onPress();
    }

    void SetOnPress(const std::function<void()>& onPress)
    {
        _onPress = onPress;
    }

private:
    std::string _text;
    std::function<void()> _onPress;
};
