#pragma once
#include <string>
#include "Utils/Archives/ISerializable.h"

namespace Ranok
{
class IArchive;

class ISettings: public ISerializable
{
public:
	ISettings(const std::string& filepath);
	~ISettings() override = default;
	
	const std::string& GetFilepath() const { return _filepath; }
	
private:
	std::string _filepath;
};

}
