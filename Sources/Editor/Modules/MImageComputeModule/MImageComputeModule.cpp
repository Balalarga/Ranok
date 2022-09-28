#include "MImageComputeModule.h"

#include "Localization/LocalizationSystem.h"

namespace Ranok
{
DEFINELOCALETEXT(ModuleName, "MimageCompute")
MImageComputeModule::MImageComputeModule():
	IEditorModule(GETLOCALETEXTSTR(ModuleName))
{
}

void MImageComputeModule::RenderWindowContent()
{
	
}
}
