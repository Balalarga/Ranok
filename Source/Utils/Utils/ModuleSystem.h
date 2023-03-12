#pragma once

#include <vector>
#include <memory>

namespace Ranok
{
class IModule
{
public:
    virtual ~IModule() = default;

    void Init() {}
    void Deinit() {}
};

template<class ModuleType>
class ModuleSystem
{
public:
    template<class T, class ...TArgs>
    T& AddModule(TArgs&& ...args)
    {
        auto module = new T(std::forward<TArgs>(args)...);
        _modules.push_back(std::unique_ptr<T>(module));
        
        if (bWasInited && bAutoInit)
            module->Init();

        return *module;
    }

    void Init()
    {
        if (bWasInited)
            return;
        
        for (auto& mod: _modules)
            mod->Init();

        bWasInited = true;
    }

    void Deinit()
    {
        if (!bWasInited)
            return;
        
        for (auto& mod: _modules)
            mod->Deinit();
        
        bWasInited = false;
    }

    template<class Predicate>
    void EnumerateModules(Predicate&& pred)
    {
        for (auto& mod: _modules)
            pred(mod.get());
    }
    
    bool IsAutoInitEnabled() const { return bAutoInit; }
    void SetAutoInit(bool state) { bAutoInit = state; }
    bool WasInited() const { return bWasInited; }

private:
    bool bAutoInit = true;
    bool bWasInited = false;
    std::vector<std::unique_ptr<ModuleType>> _modules;
};
}
