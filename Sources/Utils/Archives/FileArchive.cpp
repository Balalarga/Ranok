#include "FileArchive.h"

#include <sstream>

#include "Utils/FileUtils.h"

namespace Ranok
{
template<class T>
void FileArchive::StreamWrite(const T& val)
{
	if (_stream.is_open() && GetMode() == ArchiveMode::Write)
		_stream << val;
}

template<class T>
void FileArchive::StreamRead(T& val)
{
	if (_stream.is_open() && GetMode() == ArchiveMode::Read)
		_stream >> val;
}

FileArchive::FileArchive(std::string filepath, ArchiveMode mode, bool bIsBinary):
	IArchive(mode),
	_filepath(std::move(filepath))
{
	if (bIsBinary)
		_stdFlags = static_cast<std::_Iosb<int>::_Openmode>(_stdFlags | std::ios_base::binary);
}

FileArchive::~FileArchive()
{
	FileArchive::Close();
}

bool FileArchive::Open()
{
	if (GetMode() == ArchiveMode::Read)
	{
		_stdFlags = static_cast<std::_Iosb<int>::_Openmode>(_stdFlags | std::ios_base::in);
	}
	else if (GetMode() == ArchiveMode::Write)
	{
		if (!Files::IsFileExists(_filepath))
			Files::CreateFile(_filepath);
		_stdFlags = static_cast<std::_Iosb<int>::_Openmode>(_stdFlags | std::ios_base::out);
	}
	
	_stream.open(_filepath, _stdFlags);
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
	if (_stream.is_open() || GetMode() == ArchiveMode::Read)
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
