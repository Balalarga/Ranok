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
	std::string GetName() const
	{
		size_t nameStart = _filepath.find_last_of('/')+1;
		if (nameStart ==  std::string::npos)
			nameStart = _filepath.find_last_of('\\')+1;
		if (nameStart ==  std::string::npos)
			nameStart = 0;
		
		return _filepath.substr(nameStart,_filepath.length()-nameStart-5); // .json removing
	}
	
	bool IsDefaultOnly() const { return _bDefaultOnly; }
	bool IsNotLoadable() const { return _bNotLoadable; }
	bool HaveGui() const { return _bHaveGui; }
	
	void Serialize(IArchive& archive) override {}
	virtual void Serialize(JsonArchive& archive) = 0;
	
	virtual bool operator!=(const IConfig& oth) { return false; }

	virtual void ShowWidgets() {}
	void OnSettingsChangedCallback(const std::function<void()>& func) { _onSettingsChanged = func; }
	
	
protected:
	void SettingsChanged() const { if (_onSettingsChanged) _onSettingsChanged(); }

	void SetNotLoadable(bool state) { _bNotLoadable = state; }
	
	
private:
	std::string _filepath;
	const bool _bDefaultOnly;
	const bool _bHaveGui;
	bool _bNotLoadable;
	std::function<void()> _onSettingsChanged;
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
template<typename T, qualifier Q>
void to_json(nlohmann::json& j, const vec<4, T, Q>& p)
{
	j = nlohmann::detail::make_array<T>(p.x, p.y, p.z, p.w);
}
template<typename T, qualifier Q>
void from_json(const nlohmann::json& j, vec<4, T, Q>& p)
{
	j.at(0).get_to(p.x);
	j.at(1).get_to(p.y);
	j.at(2).get_to(p.z);
	j.at(3).get_to(p.w);
}
}