#pragma once

#include <functional>

#include <SDL.h>


enum class KeyState
{
    Pressed, Released, Repeated
};


class InputSystem
{
    friend class ISdlWindow;
public:
    static InputSystem& Get();

    void Add(SDL_Scancode code, const std::function<void(const KeyState&)>& callback);
    bool Remove(SDL_Scancode code, const std::function<void(const KeyState&)>& callback);


protected:
    InputSystem() = default;

    void OnStateChange(SDL_Scancode code, KeyState state);


private:
    std::vector<std::function<void(const KeyState&)>> KeyCallbacks[SDL_NUM_SCANCODES];
};
