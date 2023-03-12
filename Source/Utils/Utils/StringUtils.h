#pragma once

#include <algorithm>
#include <string>


namespace Ranok::StringUtils
{
inline bool Compare(const std::string& a, const std::string& b, bool bCaseSense = false)
{
	if (bCaseSense)
		return std::ranges::equal(a, b, [](char a, char b) { return tolower(a) == tolower(b); });
	return a == b;
}
}