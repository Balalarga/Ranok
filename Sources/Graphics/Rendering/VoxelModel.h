#pragma once
#include "Object.h"

#include "Graphics/Materials/IMaterial.h"

#include "Model/LinearGradient.h"
#include "Model/Space.h"

#include "Utils/FlatArray.h"

namespace Ranok
{
class VoxelMaterial: public IMaterial
{
public:
	VoxelMaterial();
	void SetupUniforms() override;
};


class VoxelModel: public Object
{
public:
	static VoxelModel* Make(const Space3D& space,
		FlatArray<MImage3D> &image,
		LinearGradient& gradient,
		size_t activeImage = 0);

	VoxelModel(Buffer vbo);

	void Update(const Space3D& space, FlatArray<std::array<double, 5>> &image, LinearGradient& gradient, size_t activeImage);

	void Render() const override;


private:
	size_t _voxelsCount{};
	size_t _voxelFilled{};
	glm::fvec4 _modelColor{};
	VoxelMaterial _material;
};
}
