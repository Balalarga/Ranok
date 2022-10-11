#pragma once

namespace Ranok
{
class Serializer;

class ISerializable
{
public:
	virtual ~ISerializable() = default;

	virtual void Serialize(Serializer& archive) = 0;
};
}
