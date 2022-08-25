#pragma once

#include <vector>
#include "glm/glm.hpp"


static glm::fvec4 ColorFromUint(unsigned char r,unsigned char g,
								unsigned char b,unsigned char a)
{
	return glm::fvec4(r/255.f, g/255.f,
					  b/255.f, a/255.f);
}

static glm::fvec4 ColorFromHex(unsigned int color)
{
	return ColorFromUint(((color >> 24) & 0xFF),
						 ((color >> 16) & 0xFF),
						 ((color >> 8) & 0xFF),
						 ((color) & 0xFF));
}


class LinearGradient
{
public:
	LinearGradient(const std::vector<glm::fvec4>& colors = {}):
		_stops(colors) {}

	glm::fvec4 GetColor(unsigned value)
	{
		// Find the "bin" that value falls in
		unsigned range = UINT_MAX;
		float step = static_cast<float>(range) / static_cast<float>(_stops.size() - 1);
		int bin = static_cast<int>(static_cast<float>(value) / step);

		// Normalize value in the interval (0,1]
		return Interpolate(_stops[bin], _stops[bin + 1], (static_cast<float>(value) - bin * step) / step);
	}

	void SetColors(const std::vector<glm::fvec4>& colors) { _stops = colors; }


protected:
	glm::fvec4 Interpolate(const glm::fvec4& c1, const glm::fvec4& c2, float normalizedValue)
	{
		if (normalizedValue <= 0.0f)
			return c1;
		
		if (normalizedValue >= 1.0f)
			return c2;

		return {
			(1.0f - normalizedValue) * c1.r + normalizedValue * c2.r,
			(1.0f - normalizedValue) * c1.g + normalizedValue * c2.g,
			(1.0f - normalizedValue) * c1.b + normalizedValue * c2.b,
			(1.0f - normalizedValue) * c1.a + normalizedValue * c2.a
		};
	}


private:
	std::vector<glm::fvec4> _stops;
};
