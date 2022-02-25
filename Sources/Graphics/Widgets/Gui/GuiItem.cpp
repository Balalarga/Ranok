#include "GuiItem.h"

GuiItem::GuiItem(const std::function<void()>& func):
    _renderFunc(func)
{

}

void GuiItem::Render()
{
    _renderFunc();
}
