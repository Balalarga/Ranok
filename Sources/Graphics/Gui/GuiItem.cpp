#include "GuiItem.h"
#include <string>

static int UnnamedCounter = 0;

GuiBase::GuiBase(const std::string &name):
   _name(name)
{
    if (_name.empty())
        _name = "#guiBase_" + std::to_string(UnnamedCounter++);
}

GuiBaseRaw::GuiBaseRaw(const std::function<void ()> &renderFunc):
    _renderFunc(renderFunc)
{

}

void GuiBaseRaw::Render()
{
    _renderFunc();
}
