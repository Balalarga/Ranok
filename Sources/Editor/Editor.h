#pragma once

#include "GuiWrap/WMenuBar.h"
#include "WindowSystem/OpenglWindow.h"
#include "ModuleSystem.h"
#include "GuiWrap/WWindow.h"

namespace Ranok
{
class WLogs;

class IEditorModule: public IModule, public WWindow
{
public:
    IEditorModule(const std::string& name):
        WWindow(name)
    {
        Close();
    }

    void SetMenuItemPtr(WMenuCheckItem* menuItem)
    {
        _menuItem = menuItem;
    }

    void OnClose() override
    {
        if (_menuItem)
            _menuItem->SetChecked(false, true);
    }

    /// Change it to false, if don't want Gui window in your's module
    const bool bHasGui = true;


private:
    WMenuCheckItem* _menuItem{};
};

class Editor: public OpenglWindow
{
public:
    static ModuleSystem<IEditorModule> EditorSystem;

	Editor();
	
	void GuiRender();

    WWindow& GetMainWindow() { return _mainWindow; }

    WLogs* GetLogWindow() { return _logWindow; }
    
private:
    WWindow _mainWindow;
    WLogs* _logWindow{};
};
}
