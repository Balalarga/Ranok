#include "JsonArchive.h"

namespace Ranok
{
JsonArchive::JsonArchive(std::string filepath, ArchiveMode mode):
	FileArchive(filepath, mode)
{
}

bool JsonArchive::Open()
{
	if (!FileArchive::Open())
		return false;

	if (GetMode() == ArchiveMode::Read)
	{
		_json = nlohmann::json::parse(GetStdStream());
		return _json.is_object() || _json.is_array();
	}

	return true;
}

void JsonArchive::Close()
{
	if (GetMode() == ArchiveMode::Write)
		Write(_json.dump(4));
	FileArchive::Close();
}
}
