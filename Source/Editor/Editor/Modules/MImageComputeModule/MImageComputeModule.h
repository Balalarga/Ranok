#pragma once
#include "../EditorModule.h"

#include "Graphics/Camera.h"
#include "Graphics/Buffers/FrameBuffer.h"

#include "Utils/LinearGradient.h"
#include "VoxelCore/Space/MultiDimSpace.h"

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
    MultiDimSpace _space;
	std::unique_ptr<VoxelModel> _model;
	Camera _camera;
};
}
