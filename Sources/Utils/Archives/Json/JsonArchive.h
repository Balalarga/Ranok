#pragma once
#include "Utils/Archives/FileArchive.h"
#include "Utils/ThirdParty/nlohmann_json.hpp"

namespace Ranok
{
class IJsonSerializable;

class JsonArchiveReader: public FileArchive
{
public:
	JsonArchiveReader(std::string filepath);

	void Read(nlohmann::json& val);
};

class JsonArchiveWriter: public FileArchive
{
public:
	JsonArchiveWriter(std::string filepath);

	void Write(const nlohmann::json& val);
};
}
