#pragma once

#include "WindowSystem/OpenglWindow.h"
#include "Utils/ModuleSystem.h"

namespace Ranok
{
class IEditorModule;

class Editor: public OpenglWindow
{
public:	
    static ModuleSystem<IEditorModule> EditorSystem;
	static Editor& Instance();
	
	~Editor();
	
	void GuiRender();
	
	void TryLoadDefaultLayout() const;
	
private:
	Editor();
};
}
