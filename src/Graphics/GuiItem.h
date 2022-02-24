#ifndef GUIITEM_H
#define GUIITEM_H

#include "imgui.h"
#include <functional>

class GuiItem
{
public:
    GuiItem(const std::function<void()>& func);
    void Render();

private:
    std::function<void()> _renderFunc;
};

#endif // GUIITEM_H
