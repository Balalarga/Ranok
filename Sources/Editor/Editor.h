#pragma once

#include "WindowSystem/OpenglWindow.h"
#include "ModuleSystem.h"

namespace Ranok
{
class IEditorModule;

class Editor: public OpenglWindow
{
public:
	struct ModuleMenuItem
	{
		std::string name;
		bool* isOpened{};
	};
	
    static ModuleSystem<IEditorModule> EditorSystem;
	static Editor& Instance();

	void GuiRender();

	void AddModuleMenuItem(ModuleMenuItem&& item);
	
private:
	Editor();
	std::vector<ModuleMenuItem> _moduleMenuItems;
};

class IEditorModule: public IModule
{
public:
	IEditorModule(const std::string& moduleName);
	
	virtual void RenderGui();
	
	std::string moduleName;
	bool bWorks = false;
};
}
