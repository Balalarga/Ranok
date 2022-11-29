#pragma once
#include <cmath>

namespace Ranok::Math
{

inline bool Equals(const double& v1, const double& v2)
{
	return std::abs(v1-v2) <= DBL_MIN;
}

inline bool Equals(const float& v1, const float& v2)
{
	return std::abs(v1-v2) <= FLT_MIN;
}

inline bool IsZero(const float& v1)
{
	return abs(v1) <= FLT_MIN;
}

inline bool IsZero(const double& v1)
{
	return abs(v1) <= DBL_MIN;
}
}
