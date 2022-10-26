#pragma once
#include "Editor/Modules/EditorModule.h"
#include "Graphics/Buffers/FrameBuffer.h"

namespace Ranok
{
class MImageComputeModule: public IEditorModule
{
public:
	MImageComputeModule();
	void RenderWindowContent() override;
	
private:
	FrameBuffer _viewport;
};
}
