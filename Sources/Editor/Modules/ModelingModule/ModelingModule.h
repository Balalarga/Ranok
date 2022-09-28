#pragma once
#include "Editor/Modules/EditorModule.h"

namespace Ranok
{
class ModelingModule: public IEditorModule
{
public:
	ModelingModule();
	
	void RenderWindowContent() override;
	
private:
	struct SubwindowSettings
	{
		
	};
};
}
