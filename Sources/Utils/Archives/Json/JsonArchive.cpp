#include "JsonArchive.h"

#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

namespace Ranok
{
JsonArchiveReader::JsonArchiveReader(std::string filepath):
	FileArchive(filepath, std::ios_base::in)
{
	
}

void JsonArchiveReader::Read(nlohmann::json& doc)
{
}

JsonArchiveWriter::JsonArchiveWriter(std::string filepath):
	FileArchive(filepath, std::ios_base::out)
{
	
}

void JsonArchiveWriter::Write(const nlohmann::json& doc)
{
}
}
