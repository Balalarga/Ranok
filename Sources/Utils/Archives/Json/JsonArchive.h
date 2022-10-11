#pragma once
#include "Utils/Archives/FileArchive.h"
#include "rapidjson/document.h"

namespace Ranok
{
class IJsonSerializable;

class JsonArchiveReader: public FileArchive
{
public:
	JsonArchiveReader(std::string filepath);

	void Read(rapidjson::Document& val);
};

class JsonArchiveWriter: public FileArchive
{
public:
	JsonArchiveWriter(std::string filepath);

	void Write(const rapidjson::Document& val);
};
}
