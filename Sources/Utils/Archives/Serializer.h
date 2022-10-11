#pragma once
#include <memory>
#include <string>
#include "IArchive.h"

namespace Ranok
{
class IArchive;

enum class SerializeMode
{
	Loading, Saving
};

class Serializer
{
public:
	template<class ArchiveType, class ...TArgs>
	static std::enable_if_t<std::derived_from<ArchiveType, IArchive>, Serializer> SaveTo(TArgs&& ...args)
	{
		auto arch = new ArchiveType(std::forward<TArgs>(args)...);
		arch->Open(ArchiveMode::Write);
		return Serializer(arch, SerializeMode::Saving);
	}
	
	template<class ArchiveType, class ...TArgs>
	static std::enable_if_t<std::derived_from<ArchiveType, IArchive>, Serializer> LoadFrom(TArgs&& ...args)
	{
		auto arch = new ArchiveType(std::forward<TArgs>(args)...);
		arch->Open(ArchiveMode::Read);
		return Serializer(arch, SerializeMode::Loading);
	}
	
	virtual ~Serializer() = default;
	
	IArchive& GetInner();
	
	const SerializeMode& GetMode() const { return _mode; }
	bool IsLoading() const { return _mode == SerializeMode::Loading; }
	bool IsSaving() const { return _mode == SerializeMode::Saving; }
	
	virtual void Serialize(int& val);
	virtual void Serialize(char& val);
	virtual void Serialize(bool& val);
	virtual void Serialize(long& val);
	virtual void Serialize(float& val);
	virtual void Serialize(double& val);
	virtual void Serialize(unsigned& val);
	virtual void Serialize(std::string& val);

	template<class T>
	Serializer& operator<<(T& val)
	{
		Serialize(val);
		return *this;
	}

private:
	Serializer(IArchive* archive, SerializeMode mode);
	
	std::unique_ptr<IArchive> _archive;
	SerializeMode _mode;


	template<class T>
	void SerializeInner(T& val);
};
}
