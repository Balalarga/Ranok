#include "JsonArchive.h"

#include <rapidjson/stringbuffer.h>
#include <rapidjson/writer.h>

namespace Ranok
{
JsonArchiveReader::JsonArchiveReader(std::string filepath):
	FileArchive(filepath, std::ios_base::in)
{
	
}

void JsonArchiveReader::Read(rapidjson::Document& doc)
{
	doc.Parse(ReadAll().c_str());
}

JsonArchiveWriter::JsonArchiveWriter(std::string filepath):
	FileArchive(filepath, std::ios_base::out)
{
	
}

void JsonArchiveWriter::Write(const rapidjson::Document& doc)
{
	if (doc.IsObject() || doc.IsArray())
	{
		rapidjson::StringBuffer buffer;
		rapidjson::Writer writer(buffer);
		doc.Accept(writer);
		FileArchive::Write(std::string(buffer.GetString()));
	}
}
}
