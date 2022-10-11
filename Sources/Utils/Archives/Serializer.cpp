#include "Serializer.h"

#include "IArchive.h"

namespace Ranok
{
Serializer::Serializer(IArchive* archive, SerializeMode mode):
	_archive(archive),
	_mode(mode)
{
	
}

IArchive& Serializer::GetInner()
{
	return *_archive;
}

template<class T>
void Serializer::SerializeInner(T& val)
{
	if (IsLoading())
		_archive->Read(val);
	else if (IsSaving())
		_archive->Write(val);
}

void Serializer::Serialize(int& val)
{
	SerializeInner(val);
}

void Serializer::Serialize(char& val)
{
	SerializeInner(val);
}

void Serializer::Serialize(bool& val)
{
	SerializeInner(val);
}

void Serializer::Serialize(long& val)
{
	SerializeInner(val);
}

void Serializer::Serialize(float& val)
{
	SerializeInner(val);
}

void Serializer::Serialize(double& val)
{
	SerializeInner(val);
}

void Serializer::Serialize(unsigned& val)
{
	SerializeInner(val);
}

void Serializer::Serialize(std::string& val)
{
	SerializeInner(val);
}
}
