#pragma once

#include "IWidget.h"

#include <functional>

class WRawWidget: public IWidget
{
public:
	WRawWidget(std::function<void()>&& renderFunc):
		_renderFunc(renderFunc)
	{
	}

	void Render() override
	{
		_renderFunc();
	}
	
private:
	std::function<void()> _renderFunc;
};