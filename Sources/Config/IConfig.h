#pragma once
#include <string>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include "Utils/Archives/ISerializable.h"
#include "Utils/ThirdParty/nlohmann_json.hpp"

namespace Ranok
{
class JsonArchive;
class IArchive;

class IConfig: public ISerializable
{
	friend class ConfigManager;
public:
	IConfig(const std::string& filepath, bool bDefaultOnly = false, bool bHasGui = false);
	~IConfig() override = default;
	
	const std::string& GetFilepath() const { return _filepath; }
	bool IsDefaultOnly() const { return _bDefaultOnly; }
	bool HaveGui() const { return _bHaveGui; }
	
	void Serialize(IArchive& archive) override {}
	virtual void Serialize(JsonArchive& archive) = 0;
	
	virtual bool operator!=(const IConfig& oth) { return false; }

	virtual void ShowWidgets() {}
	
private:
	std::string _filepath;
	const bool _bDefaultOnly;
	const bool _bHaveGui;
};

}


namespace glm
{
template<typename T, qualifier Q>
void to_json(nlohmann::json& j, const vec<2, T, Q>& p)
{
	j = nlohmann::detail::make_array<T>(p.x, p.y);
}
template<typename T, qualifier Q>
void from_json(const nlohmann::json& j, vec<2, T, Q>& p)
{
	j.at(0).get_to(p.x);
	j.at(1).get_to(p.y);
}
template<typename T, qualifier Q>
void to_json(nlohmann::json& j, const vec<3, T, Q>& p)
{
	j = nlohmann::detail::make_array<T>(p.x, p.y, p.z);
}
template<typename T, qualifier Q>
void from_json(const nlohmann::json& j, vec<3, T, Q>& p)
{
	j.at(0).get_to(p.x);
	j.at(1).get_to(p.y);
	j.at(2).get_to(p.z);
}
}