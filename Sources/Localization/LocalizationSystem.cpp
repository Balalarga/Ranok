#include "LocalizationSystem.h"
#include <memory>
#include "Settings/ISettings.h"
#include "Settings/SettingsManager.h"
#include "Utils/Archives/Json/JsonArchive.h"


namespace Ranok
{
class LocalizationSettings: public ISettings
{
public:
    LocalizationSettings(): ISettings("Localization/LocalizationConfig", true)
    {
    }
    
    void Serialize(JsonArchive& archive) override
    {
        archive.Serialize("activeLocaleName", localeName);
        archive.Serialize("locales", locales);
    }
    
    const std::string defaultLocaleName = "en";
    
    std::string localeName = defaultLocaleName;
    LocalizationSystem::LocalesMap locales{{defaultLocaleName, {}}};
};

std::shared_ptr<LocalizationSettings> settings;

LocalizationSystem::LocalizationSystem()
{
    settings = SettingsManager::Instance().CreateSettings<LocalizationSettings>();
}

LocalizationSystem &LocalizationSystem::Get()
{
    static LocalizationSystem self;
    return self;
}

LocalizationSystem::~LocalizationSystem()
{
    SettingsManager::Instance().SaveSetting(settings.get());
}

std::string LocalizationSystem::GetDefaultText(const std::string& id)
{
    return settings->locales[settings->defaultLocaleName][id];
}

bool LocalizationSystem::AddLocale(const std::string &name)
{
    if (FindLocale(name))
        return false;

    settings->locales[name] = settings->locales[settings->defaultLocaleName];
    return true;
}

bool LocalizationSystem::EnableLocale(const std::string& name)
{
    if (FindLocale(name))
    {
        settings->localeName = name;
        return true;
    }
    return false;
}

const std::string& LocalizationSystem::GetActiveLocale()
{
    return settings->localeName;
}

std::string LocalizationSystem::AddText(const std::string& tag, const std::string& text)
{
    std::map<std::string, std::string>& currentLocale = settings->locales[settings->localeName];
    if (currentLocale.contains(tag))
        return currentLocale[tag];
    
    currentLocale.insert({tag, text});
    return tag;
}

std::string LocalizationSystem::GetText(const std::string& id) const
{
    std::map<std::string, std::string>& currnetLocale = settings->locales[settings->localeName];
    return currnetLocale.contains(id) ? currnetLocale[id] : "<unknown_id>";
}

std::map<std::string, std::string>* LocalizationSystem::FindLocale(const std::string &name)
{
    auto it = settings->locales.find(name);
    if (it == settings->locales.end())
        return nullptr;

    return &it->second;
}
}
