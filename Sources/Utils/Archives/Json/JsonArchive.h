#pragma once
#include "Utils/Archives/FileArchive.h"
#include "Utils/ThirdParty/nlohmann_json.hpp"


namespace Ranok
{
class IJsonSerializable;

class JsonArchive: public FileArchive
{
public:
	JsonArchive(std::string filepath, ArchiveMode mode);

	bool Open() override;
	void Close() override;
	
	using FileArchive::Read;
	using FileArchive::Write;

	template<class T>
	void Serialize(const std::string& tag, T& val)
	{
		if (GetMode() == ArchiveMode::Read)
		{
			val = _json[tag];
		}
		else if (GetMode() == ArchiveMode::Write)
		{
			_json[tag] = val;
		}
	}
	
private:
	nlohmann::json _json;
};
}
