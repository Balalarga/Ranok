#pragma once

#include <map>
#include <string>
#include <vector>

#define DEFINE_LOCTEXT(tagId, text) static size_t tagId = LocalizationSystem::Get().AddText(#tagId, text);
#define LOCTEXT(tagId) LocalizationSystem::Get().GetText(tagId).c_str()
#define LOCTEXTSTR(tagId) LocalizationSystem::Get().GetText(tagId)
#define DEFAULT_LOCTEXT(tagId) LocalizationSystem::Get().GetDefaultText(tagId).c_str()
#define DEFAULT_LOCTEXTSTR(tagId) LocalizationSystem::Get().GetDefaultText(tagId)

namespace Ranok
{
class LocalizationSystem
{
public:
    using LocalesMap = std::map<std::string, std::vector<std::string>>;
    static LocalizationSystem& Get();

    bool AddLocale(const std::string& name);
    bool EnableLocale(const std::string& name);

    const std::string& GetActiveLocale();

    size_t AddText(const std::string& tag, const std::string& text);
    std::string GetText(const size_t& id) const;
    std::string GetDefaultText(const size_t& id);

private:
    LocalizationSystem();
    std::vector<std::string>* FindLocale(const std::string& name);
};
}
