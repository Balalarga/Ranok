#pragma once
#if _WINDOWS
#include <windows.h>
#endif
#include <string>

namespace Ranok
{

std::string OpenFileDialog(const LPCSTR& filters = "*.*");
std::string SaveFileDialog(const LPCSTR& filters = "*.*");

}
