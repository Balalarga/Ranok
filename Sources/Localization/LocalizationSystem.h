#pragma once

#include <map>
#include <string>
#include <vector>

#define DEFINE_LOCTEXT(tagId, text) static std::string tagId#__FILE__ = LocalizationSystem::Get().AddText(#tagId, text);
#define LOCTEXT(tagId) LocalizationSystem::Get().GetText(tagId#__FILE__).c_str()
#define LOCTEXTSTR(tagId) LocalizationSystem::Get().GetText(tagId#__FILE__)
#define DEFAULT_LOCTEXT(tagId) LocalizationSystem::Get().GetDefaultText(tagId#__FILE__).c_str()
#define DEFAULT_LOCTEXTSTR(tagId) LocalizationSystem::Get().GetDefaultText(tagId#__FILE__)

namespace Ranok
{
class LocalizationSystem
{
public:
    using LocalesMap = std::map<std::string, std::map<std::string, std::string>>;
    static LocalizationSystem& Get();

    ~LocalizationSystem();

    bool AddLocale(const std::string& name);
    bool EnableLocale(const std::string& name);

    const std::string& GetActiveLocale();

    std::string AddText(const std::string& tag, const std::string& text);
    std::string GetText(const std::string& id) const;
    std::string GetDefaultText(const std::string& id);

private:
    LocalizationSystem();
    std::map<std::string, std::string>* FindLocale(const std::string& name);
};
}
