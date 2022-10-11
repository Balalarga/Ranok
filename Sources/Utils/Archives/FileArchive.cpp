#include "FileArchive.h"

#include <sstream>

namespace Ranok
{
template<class T>
void FileArchive::StreamWrite(const T& val)
{
	if (_stream.is_open() && _stdFlags & std::ios_base::out)
		_stream << val;
}

template<class T>
void FileArchive::StreamRead(T& val)
{
	if (_stream.is_open() && _stdFlags & std::ios_base::in)
		_stream >> val;
}

FileArchive::FileArchive(std::string filepath, std::_Iosb<int>::_Openmode stdFlags):
	_filepath(std::move(filepath)),
	_stdFlags(stdFlags)
{
}

FileArchive::~FileArchive()
{
	FileArchive::Close();
}

bool FileArchive::Open(ArchiveMode mode)
{
	_stream.open(_filepath,
		_stdFlags | (mode == ArchiveMode::Read ? std::ios_base::in : std::ios_base::out));
	return _stream.is_open();
}

void FileArchive::Close()
{
	_stream.close();
}

void FileArchive::Reload()
{
	if (_stream.is_open())
	{
		_stream.clear();
		_stream.seekg(0);
	}
}

std::string FileArchive::ReadAll() const
{
	if (_stream.is_open() || _stdFlags & std::ios_base::in)
	{
		std::stringstream stream;
		stream << _stream.rdbuf();
		return stream.str();
	}
	
	return {};
}

void FileArchive::Write(const int& val)
{
	StreamWrite(val);
}

void FileArchive::Write(const char& val)
{
	StreamWrite(val);
}

void FileArchive::Write(const bool& val)
{
	StreamWrite(val);
}

void FileArchive::Write(const long& val)
{
	StreamWrite(val);
}

void FileArchive::Write(const float& val)
{
	StreamWrite(val);
}

void FileArchive::Write(const double& val)
{
	StreamWrite(val);
}

void FileArchive::Write(const unsigned& val)
{
	StreamWrite(val);
}

void FileArchive::Write(const std::string& val)
{
	StreamWrite(val);
}

void FileArchive::Read(int& val)
{
	StreamRead(val);
}

void FileArchive::Read(char& val)
{
	StreamRead(val);
}

void FileArchive::Read(bool& val)
{
	StreamRead(val);
}

void FileArchive::Read(long& val)
{
	StreamRead(val);
}

void FileArchive::Read(float& val)
{
	StreamRead(val);
}

void FileArchive::Read(double& val)
{
	StreamRead(val);
}

void FileArchive::Read(unsigned& val)
{
	StreamRead(val);
}

void FileArchive::Read(std::string& val)
{
	StreamRead(val);
}
}
