﻿#pragma once
#include <string>
#include "Utils/Archives/ISerializable.h"

namespace Ranok
{
class JsonArchive;
class IArchive;

class IConfig: public ISerializable
{
public:
	IConfig(const std::string& filepath, bool bDefaultOnly = false);
	~IConfig() override = default;
	
	const std::string& GetFilepath() const { return _filepath; }
	bool IsDefaultOnly() const { return _bDefaultOnly; }
	
	void Serialize(IArchive& archive) override {}
	virtual void Serialize(JsonArchive& archive) = 0;
	
	
private:
	std::string _filepath;
	const bool _bDefaultOnly;
};

}