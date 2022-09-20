#pragma once

#include <map>
#include <string>
#include <vector>

#define DEFINELOCALETEXT(tagId, text) static size_t tagId = LocalizationSystem::Get().AddText(#tagId, text);
#define GETLOCALETEXT(tagId) LocalizationSystem::Get().GetText(tagId)

namespace Ranok
{
class LocalizationSystem
{
public:
    using LocalesMap = std::map<std::string, std::vector<std::string>>;
    static LocalizationSystem& Get();

    bool AddLocale(const std::string& name);
    bool EnableLocale(const std::string& name);

    const LocalesMap& GetLocales() const { return _locales; }
    const std::string& GetActiveLocale() { return _currentLocaleName; }

    size_t AddText(const std::string& tag, const std::string& text);
    std::string GetText(size_t id) { return id < _currentLocale->size() ? _currentLocale->at(id) : "<unknown_id>"; }

private:
    LocalizationSystem();
    std::vector<std::string>* FindLocale(const std::string& name);

    std::string _currentLocaleName;
    LocalesMap _locales;
    std::vector<std::string>* _currentLocale{};
    std::vector<std::string> _textNames;
};
}
