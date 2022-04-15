#pragma once

#include <string>
#include <imgui.h>
#include <glm.hpp>


template<class T>
struct Setting
{
    Setting(const std::string& name = ""):
        Name(name)
    {}
    std::string Name;
    T Value;
};
