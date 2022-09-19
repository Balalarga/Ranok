#pragma once

#include "imgui.h"

class IWidget
{
public:
    virtual ~IWidget() = default;

    virtual void Render() = 0;
};