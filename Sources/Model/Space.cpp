#include "Space.h"

namespace Ranok
{
Space3D::Space3D(const std::vector<double> &centerPoint,
	  const std::vector<double> &size,
	  const size_t& recursiveDepth)
{
	assert(centerPoint.size() == size.size());

	_size = size;
	_centerPoint = centerPoint;
	_partition.resize(centerPoint.size(), std::pow(2, recursiveDepth));
	_startPoint.resize(centerPoint.size());
	UpdateStartPoint();
}
Space3D::Space3D(const std::vector<double>& centerPoint,
	  const std::vector<double>& size,
	  const std::vector<size_t>& partition)
{
	assert(centerPoint.size() == size.size());
	assert(partition.size() == size.size());

	_size = size;
	_partition = partition;
	_centerPoint = centerPoint;
	_startPoint.resize(centerPoint.size());
	UpdateStartPoint();
}
std::vector<double> Space3D::GetUnitSize() const
{
	std::vector<double> unitSizes(_size.size());

	for (size_t i = 0; i < _size.size(); ++i)
		unitSizes[i] = _size[i] / _partition[i];

	return unitSizes;
}
std::vector<float> Space3D::GetPoint(size_t id) const
{
	auto unitSize = GetUnitSize();
	std::vector<float> point(3);
	point[0] = _startPoint[0] + unitSize[0] * (id / ( _partition[2] * _partition[1] ));
	point[1] = _startPoint[1] + unitSize[1] * ((id / _partition[2] ) % _partition[1]);
	point[2] = _startPoint[2] + unitSize[2] * (id % _partition[2]);
	return point;
}

void Space3D::SetStartPoint(const std::vector<double>& point) {
	_startPoint = point;
	UpdateCenterPoint();
}

void Space3D::SetCenterPoint(const std::vector<double>& point) {
	_centerPoint = point;
	UpdateStartPoint();
}

void Space3D::UpdateCenterPoint() {
	_centerPoint.resize(_size.size());
	for(size_t i = 0; i < _size.size(); ++i)
		_centerPoint[i] = _startPoint[i] + _size[i] / 2.f;
}

void Space3D::UpdateStartPoint() {
	_startPoint.resize(_size.size());
	for(size_t i = 0; i < _size.size(); ++i)
		_startPoint[i] = _centerPoint[i] - _size[i] / 2.f;
}

std::ofstream& operator<<(std::ofstream& stream, Space3D& space)
{
	size_t dims = space._size.size();
	stream.write((char*)&dims, sizeof(dims));
	stream.write((char*)&space._size[0], sizeof(space._size[0]) * space._size.size());
	stream.write((char*)&space._centerPoint[0], sizeof(space._centerPoint[0]) * space._centerPoint.size());
	stream.write((char*)&space._partition[0], sizeof(space._partition[0]) * space._partition.size());
	return stream;
}

std::ifstream& operator>>(std::ifstream& stream, Space3D& space)
{
	size_t dims;
	stream.read((char*)&dims, sizeof(dims));

	space._size.resize(dims);
	space._centerPoint.resize(dims);
	space._startPoint.resize(dims);
	space._partition.resize(dims);

	stream.read((char*)&space._size[0], sizeof(space._size[0]) * dims);
	stream.read((char*)&space._centerPoint[0], sizeof(space._centerPoint[0]) * dims);
	stream.read((char*)&space._partition[0], sizeof(space._partition[0]) * dims);
	space.UpdateStartPoint();

	return stream;
}
}
