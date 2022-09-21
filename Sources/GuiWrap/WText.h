#pragma once
#include <string>
#include <vector>
#include "IWidget.h"

namespace Ranok
{
class WText: public IWidget
{
public:
    void AddLine(std::string&& line)
    {
        _lines.push_back(line);
    }
    
    void Render() override
    {
    }

private:
    std::vector<std::string> _lines;
};
}
