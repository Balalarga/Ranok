#include "LocalizationSystem.h"
#include <memory>
#include "Config/ConfigManager.h"
#include "Utils/Archives/Json/JsonArchive.h"


namespace Ranok
{
class LocalizationConfig: public IConfig
{
public:
    LocalizationConfig(): IConfig("Localization/LocalizationConfig", true)
    {
    }
    
    void Serialize(JsonArchive& archive) override
    {
        // TODO
        // archive.Serialize("localeIndex", localeIndex);
        // archive.Serialize("activeLocaleName", localeName);
        // archive.Serialize("locales", locales);
    }
    
    const std::string defaultLocaleName = "en";
    
    std::string localeName = defaultLocaleName;
    std::map<std::string, size_t> localeIndex;
    LocalizationSystem::LocalesMap locales{{defaultLocaleName, {}}};
};

std::shared_ptr<LocalizationConfig> config;

LocalizationSystem::LocalizationSystem()
{
    config = ConfigManager::Instance().CreateConfigs<LocalizationConfig>();
}

LocalizationSystem &LocalizationSystem::Get()
{
    static LocalizationSystem self;
    return self;
}

LocalizationSystem::~LocalizationSystem()
{
    ConfigManager::Instance().SaveConfig(config.get());
}

std::string LocalizationSystem::GetDefaultText(const size_t& id)
{
    return config->locales[config->defaultLocaleName][id];
}

bool LocalizationSystem::AddLocale(const std::string &name)
{
    if (FindLocale(name))
        return false;

    config->locales[name] = config->locales[config->defaultLocaleName];
    return true;
}

bool LocalizationSystem::EnableLocale(const std::string& name)
{
    if (FindLocale(name))
    {
        config->localeName = name;
        return true;
    }
    return false;
}

const std::string& LocalizationSystem::GetActiveLocale()
{
    return config->localeName;
}

size_t LocalizationSystem::AddText(const std::string& tag, const std::string& text)
{
    std::vector<std::string>& currentLocale = config->locales[config->localeName];
    auto it = config->localeIndex.find(tag);
    if (it != config->localeIndex.end())
        return it->second;
    currentLocale.push_back(text);
    config->localeIndex[tag] = currentLocale.size();
    return currentLocale.size()-1;
}

std::string LocalizationSystem::GetText(const size_t& id) const
{
    std::vector<std::string>& currnetLocale = config->locales[config->localeName];
    return currnetLocale.size() > id ? currnetLocale[id] : "<unknown_id>";
}

std::vector<std::string>* LocalizationSystem::FindLocale(const std::string &name)
{
    auto it = config->locales.find(name);
    if (it == config->locales.end())
        return nullptr;

    return &it->second;
}
}
