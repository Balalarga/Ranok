#pragma once
#include "Utils/Archives/FileArchive.h"
#include "Utils/Json/nlohmann_json.hpp"


namespace Ranok
{
class IJsonSerializable;

class JsonArchive: public FileArchive
{
public:
	JsonArchive(std::string filepath, ArchiveMode mode);
	~JsonArchive() override;

	bool Open() override;
	void Close() override;
	
	using FileArchive::Read;
	using FileArchive::Write;

	template<class T>
	bool Serialize(const std::string& tag, T& val)
	{
		if (GetMode() == ArchiveMode::Read)
		{
			if (!_json.contains(tag))
				return false;
			
			val = _json[tag].get<T>();
		}
		else if (GetMode() == ArchiveMode::Write)
		{
			_json[tag] = val;
		}
		return true;
	}

private:
	nlohmann::json _json;
};
}
