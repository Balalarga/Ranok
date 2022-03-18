#include "GuiItem.h"


GuiBase::GuiBase(const std::string &name):
   _name(name)
{

}

GuiBaseRaw::GuiBaseRaw(const std::function<void ()> &renderFunc):
    GuiBase(""),
    _renderFunc(renderFunc)
{

}

void GuiBaseRaw::Render()
{
    _renderFunc();
}
