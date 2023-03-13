#pragma once
#include <vector>
#include <fstream>
#include <numeric>


namespace Ranok
{
// Packing for opencl system
#ifdef __GNUC__
#define PACK( __Declaration__ ) __Declaration__ __attribute__((__packed__))
#endif
#ifdef _MSC_VER
#define PACK( __Declaration__ ) __pragma( pack(push, 1) ) __Declaration__ __pragma( pack(pop))
#endif

PACK(struct MImage3D
{
	double c[5];
	char zone;
});

class MultiDimSpace
{
public:
	MultiDimSpace() = default;
    MultiDimSpace(const std::vector<double> &centerPoint, const std::vector<double> &size, const size_t& recursiveDepth);
    MultiDimSpace(const std::vector<double>& centerPoint, const std::vector<double>& size, const std::vector<size_t>& partition);
    MultiDimSpace(const std::vector<float> &centerPoint, const std::vector<float> &size, const size_t& recursiveDepth);
    MultiDimSpace(const std::vector<float>& centerPoint, const std::vector<float>& size, const std::vector<size_t>& partition);
    MultiDimSpace(const MultiDimSpace &oth) = default;
    virtual ~MultiDimSpace() = default;

    const std::vector<double>& GetSize() const { return _size; }
    const std::vector<double>& GetCentral() const { return _centerPoint; }
    const std::vector<size_t>& GetPartition() const { return _partition; }
    const std::vector<double>& GetStartPoint() const { return _startPoint; }
    size_t GetTotalPartition() const { return std::accumulate(_partition.begin(), _partition.end(), 1, std::multiplies()); }


    std::vector<double> GetUnitSize() const;
    std::vector<float> GetPoint(size_t id) const;

    void SetSize(const std::vector<double>& size) { _size = size; }
    void SetPartition(const std::vector<size_t>& partition) { _partition = partition; }
    void SetPartition(const size_t& partition) { _partition = { partition, partition, partition }; }
    void SetStartPoint(const std::vector<double>& point);
    void SetCenterPoint(const std::vector<double>& point);
    
    friend std::ofstream& operator<<(std::ofstream &stream, MultiDimSpace& space);
    friend std::ifstream& operator>>(std::ifstream& stream, MultiDimSpace &space);
    
    
protected:
    void UpdateCenterPoint();
    void UpdateStartPoint();
    
    
private:
    std::vector<double> _size;
    std::vector<double> _centerPoint;
    std::vector<double> _startPoint;
    std::vector<size_t> _partition;
};
}
