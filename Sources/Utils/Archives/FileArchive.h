#pragma once
#include <fstream>
#include "IArchive.h"

namespace Ranok
{
class FileArchive: public IArchive
{
public:
	FileArchive(std::string filepath, ArchiveMode mode);
	~FileArchive() override;
	
	bool Open() override;
	void Close() override;

	void Reload() override;

	void Write(const int& val) override;
	void Write(const char& val) override;
	void Write(const bool& val) override;
	void Write(const long& val) override;
	void Write(const float& val) override;
	void Write(const double& val) override;
	void Write(const unsigned& val) override;
	void Write(const std::string& val) override;
	
	void Read(int& val) override;
	void Read(char& val) override;
	void Read(bool& val) override;
	void Read(long& val) override;
	void Read(float& val) override;
	void Read(double& val) override;
	void Read(unsigned& val) override;
	void Read(std::string& val) override;
	
	std::string ReadAll() const;

	const std::string& GetFilepath() const { return _filepath; }
	
	template<class T>
	void Serialize(T& val)
	{
		if (GetMode() == ArchiveMode::Read)
			Read(val);
		else if (GetMode() == ArchiveMode::Write)
			Write(val);
	}
	
protected:
	std::fstream& GetStdStream() { return _stream; }
	
	
private:
	const std::string _filepath;
	std::_Iosb<int>::_Openmode _stdFlags{};
	
	std::fstream _stream;
	
	template<class T>
	void StreamWrite(const T& val);
	
	template<class T>
	void StreamRead(T& val);
};
}
