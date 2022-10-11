#pragma once
#include <string>

namespace Ranok
{
enum class ArchiveMode
{
	Read, Write
};

class IArchive
{
public:
	virtual ~IArchive() = default;
	
	virtual bool Open(ArchiveMode mode) = 0;
	virtual void Close() = 0;
	
	virtual void Reload() {}
	
	virtual void Write(const int& val) = 0;
	virtual void Write(const char& val) = 0;
	virtual void Write(const bool& val) = 0;
	virtual void Write(const long& val) = 0;
	virtual void Write(const float& val) = 0;
	virtual void Write(const double& val) = 0;
	virtual void Write(const unsigned& val) = 0;
	virtual void Write(const std::string& val) = 0;
	
	virtual void Read(int& val) = 0;
	virtual void Read(char& val) = 0;
	virtual void Read(bool& val) = 0;
	virtual void Read(long& val) = 0;
	virtual void Read(float& val) = 0;
	virtual void Read(double& val) = 0;
	virtual void Read(unsigned& val) = 0;
	virtual void Read(std::string& val) = 0;
};
}
