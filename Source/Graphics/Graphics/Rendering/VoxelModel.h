#pragma once
#include "Object.h"

#include "Graphics/Materials/IMaterial.h"

#include "Utils/LinearGradient.h"
#include "VoxelCore/Space/MultiDimSpace.h"

#include "Utils/FlatArray.h"

class Camera;

namespace Ranok
{
class VoxelMaterial: public IMaterial
{
public:
	VoxelMaterial();
	void SetupUniforms() override;

	void SetCameraMatrix(glm::mat4 mat);

private:
	glm::mat4 _cameraVPmatrix{1.f};
};


class VoxelModel: public Object
{
public:
	static VoxelModel* Make(const MultiDimSpace& space,
		FlatArray<MImage3D> &image,
		LinearGradient& gradient,
		size_t activeImage = 0);

	VoxelModel(Buffer vbo);

	void Update(const MultiDimSpace& space, FlatArray<std::array<double, 5>> &image, LinearGradient& gradient, size_t activeImage);

	void Render() const override;

	VoxelMaterial& Material() { return _material; }


private:
	size_t _voxelsCount{};
	size_t _voxelFilled{};
	glm::fvec4 _modelColor{};
	VoxelMaterial _material;
};
}
