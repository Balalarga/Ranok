#pragma once

namespace Ranok
{
class IArchive;

class ISerializable
{
public:
	virtual ~ISerializable() = default;

	virtual void Serialize(IArchive& archive) = 0;
};
}
