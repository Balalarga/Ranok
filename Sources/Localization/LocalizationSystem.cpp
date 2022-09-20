#include "LocalizationSystem.h"


namespace Ranok
{
LocalizationSystem::LocalizationSystem():
    _currentLocaleName("en")
{
    _locales[_currentLocaleName] = {};
    _currentLocale = &_locales.begin()->second;
}

LocalizationSystem &LocalizationSystem::Get()
{
    static LocalizationSystem self;
    return self;
}

bool LocalizationSystem::AddLocale(const std::string &name)
{
    if (FindLocale(name))
        return false;

    _locales[name] = _locales.begin()->second;
    return true;
}

bool LocalizationSystem::EnableLocale(const std::string &name)
{
    if (std::vector<std::string>* locale = FindLocale(name))
    {
        _currentLocale = locale;
        _currentLocaleName = name;
        return true;
    }
    return false;
}

size_t LocalizationSystem::AddText(const std::string& tag, const std::string& text)
{
    _currentLocale->push_back(text);
    _textNames.push_back(tag);
    return _currentLocale->size()-1;
}

std::vector<std::string> *LocalizationSystem::FindLocale(const std::string &name)
{
    auto it = _locales.find(name);
    if (it == _locales.end())
        return nullptr;

    return &it->second;
}
}
