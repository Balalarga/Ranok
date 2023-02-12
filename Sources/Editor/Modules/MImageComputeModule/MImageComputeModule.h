#pragma once
#include "Editor/Modules/EditorModule.h"
#include "Graphics/Buffers/FrameBuffer.h"

#include "Model/LinearGradient.h"
#include "Model/Space.h"

#include "Utils/FlatArray.h"

namespace Ranok
{
class VoxelModel;

class MImageComputeModule: public IEditorModule
{
public:
	MImageComputeModule();
	void RenderWindowContent() override;
	
private:
	FrameBuffer _viewport;
	LinearGradient _imageGradient;
	FlatArray<MImage3D> _imageData;
    Space3D _space;
	std::unique_ptr<VoxelModel> _model;
};
}
