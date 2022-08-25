#pragma once
#include <cassert>
#include <vector>
#include <cmath>
#include <fstream>


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

class Space3D
{
	Space3D() = default;
    Space3D(const std::vector<double> &centerPoint, const std::vector<double> &size, const size_t& recursiveDepth);
    Space3D(const std::vector<double>& centerPoint, const std::vector<double>& size, const std::vector<size_t>& partition);
    Space3D(const Space3D &oth) = default;
    virtual ~Space3D() = default;

    const std::vector<double>& GetSize() const { return _size; }
    const std::vector<double>& GetCentral() const { return _centerPoint; }
    const std::vector<size_t>& GetPartition() const { return _partition; }
    const std::vector<double>& GetStartPoint() const { return _startPoint; }
    size_t GetTotalPartition() const
    {
        size_t total = 1;
        for (auto& i: _partition)
            total *= i;
        return total;
    }


    std::vector<double> GetUnitSize() const;
    std::vector<float> GetPoint(size_t id) const;

    void SetSize(const std::vector<double>& size) { _size = size; }
    void SetPartition(const std::vector<size_t>& partition) { _partition = partition; }
    void SetPartition(const size_t& partition) { _partition = { partition, partition, partition }; }
    void SetStartPoint(const std::vector<double>& point);
    void SetCenterPoint(const std::vector<double>& point);
    
    friend std::ofstream& operator << (std::ofstream &stream, Space3D& space);
    friend std::ifstream& operator >> (std::ifstream& stream, Space3D &space);
    
    
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
