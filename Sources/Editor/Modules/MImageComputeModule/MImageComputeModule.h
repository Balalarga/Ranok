#pragma once
#include "Editor/Modules/EditorModule.h"

namespace Ranok
{
class MImageComputeModule: public IEditorModule
{
public:
	MImageComputeModule();
	void RenderWindowContent() override;
};
}
