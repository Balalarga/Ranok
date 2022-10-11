#pragma once
#include <string>

namespace Ranok
{

std::string OpenFileDialog(const std::string& filters = "*.*");
std::string SaveFileDialog(const std::string& filters = "*.*");

}
