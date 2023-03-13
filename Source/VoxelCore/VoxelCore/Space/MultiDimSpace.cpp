#include "MultiDimSpace.h"

namespace Ranok
{
MultiDimSpace::MultiDimSpace(const std::vector<double> &centerPoint,
	  const std::vector<double> &size,
	  const size_t& recursiveDepth)
{
	if (centerPoint.size() != size.size())
		return;

	_size = size;
	_centerPoint = centerPoint;
	_partition.resize(centerPoint.size(), std::pow(2, recursiveDepth));
	_startPoint.resize(centerPoint.size());
	UpdateStartPoint();
}
MultiDimSpace::MultiDimSpace(const std::vector<double>& centerPoint,
	  const std::vector<double>& size,
	  const std::vector<size_t>& partition)
{
	if (centerPoint.size() != size.size() || partition.size() != size.size())
		return;

	_size = size;
	_partition = partition;
	_centerPoint = centerPoint;
	_startPoint.resize(centerPoint.size());
	UpdateStartPoint();
}

MultiDimSpace::MultiDimSpace(const std::vector<float>& centerPoint, const std::vector<float>& size, const size_t& recursiveDepth)
{
	if (centerPoint.size() != size.size())
		return;

	for (const float& val : size)
		_size.push_back(static_cast<double>(val));

	for (const float& val : centerPoint)
		_centerPoint.push_back(static_cast<double>(val));
	_partition.resize(centerPoint.size(), std::pow(2, recursiveDepth));
	_startPoint.resize(centerPoint.size());
	UpdateStartPoint();
}

MultiDimSpace::MultiDimSpace(const std::vector<float>& centerPoint,
	const std::vector<float>& size,
	const std::vector<size_t>& partition)
{
	if (centerPoint.size() != size.size() || partition.size() != size.size())
		return;

	for (const float& val : size)
		_size.push_back(static_cast<double>(val));

	for (const float& val : centerPoint)
		_centerPoint.push_back(static_cast<double>(val));
	_partition = partition;
	_startPoint.resize(centerPoint.size());
	UpdateStartPoint();
}

std::vector<double> MultiDimSpace::GetUnitSize() const
{
	std::vector<double> unitSizes(_size.size());

	for (size_t i = 0; i < _size.size(); ++i)
		unitSizes[i] = _size[i] / _partition[i];

	return unitSizes;
}
std::vector<float> MultiDimSpace::GetPoint(size_t id) const
{
	const std::vector<double> unitSize = GetUnitSize();
	std::vector<float> point(3);
	point[0] = _startPoint[0] + unitSize[0] * (id / ( _partition[2] * _partition[1] ));
	point[1] = _startPoint[1] + unitSize[1] * ((id / _partition[2] ) % _partition[1]);
	point[2] = _startPoint[2] + unitSize[2] * (id % _partition[2]);
	return point;
}

void MultiDimSpace::SetStartPoint(const std::vector<double>& point)
{
	_startPoint = point;
	UpdateCenterPoint();
}

void MultiDimSpace::SetCenterPoint(const std::vector<double>& point)
{
	_centerPoint = point;
	UpdateStartPoint();
}

void MultiDimSpace::UpdateCenterPoint()
{
	_centerPoint.resize(_size.size());
	for(size_t i = 0; i < _size.size(); ++i)
		_centerPoint[i] = _startPoint[i] + _size[i] / 2.f;
}

void MultiDimSpace::UpdateStartPoint()
{
	_startPoint.resize(_size.size());
	for(size_t i = 0; i < _size.size(); ++i)
		_startPoint[i] = _centerPoint[i] - _size[i] / 2.f;
}

std::ofstream& operator<<(std::ofstream& stream, MultiDimSpace& space)
{
	size_t dims = space._size.size();
	stream.write(reinterpret_cast<char*>(&dims), sizeof(dims));
	stream.write(reinterpret_cast<char*>(space._size.data()), sizeof(space._size[0]) * space._size.size());
	stream.write(reinterpret_cast<char*>(space._centerPoint.data()), sizeof(space._centerPoint[0]) * space._centerPoint.size());
	stream.write(reinterpret_cast<char*>(space._partition.data()), sizeof(space._partition[0]) * space._partition.size());
	return stream;
}

std::ifstream& operator>>(std::ifstream& stream, MultiDimSpace& space)
{
	size_t dims;
	stream.read(reinterpret_cast<char*>(&dims), sizeof(dims));

	space._size.resize(dims);
	space._centerPoint.resize(dims);
	space._startPoint.resize(dims);
	space._partition.resize(dims);

	stream.read(reinterpret_cast<char*>(space._size.data()), sizeof(space._size[0]) * dims);
	stream.read(reinterpret_cast<char*>(space._centerPoint.data()), sizeof(space._centerPoint[0]) * dims);
	stream.read(reinterpret_cast<char*>(space._partition.data()), sizeof(space._partition[0]) * dims);
	space.UpdateStartPoint();

	return stream;
}
}
