#include "InputSystem.h"


InputSystem& InputSystem::Get()
{
    static InputSystem selfInstance;
    return selfInstance;
}

void InputSystem::Add(SDL_Scancode code, const std::function<void(const KeyState&)>& callback)
{
    KeyCallbacks[code].push_back(callback);
}

bool InputSystem::Remove(SDL_Scancode code, const std::function<void(const KeyState&)>& callback)
{
    for (int i = 0; i < KeyCallbacks[code].size(); ++i)
    {
        if (KeyCallbacks[code][i].target<void(const KeyState&)>() == callback.target<void(const KeyState&)>())
        {
            KeyCallbacks[code].erase(KeyCallbacks[code].begin() + i);
            return true;
        }
    }

    return false;
}


void InputSystem::OnStateChange(SDL_Scancode code, KeyState state)
{
    for (auto& func : KeyCallbacks[code])
        func(state);
}
