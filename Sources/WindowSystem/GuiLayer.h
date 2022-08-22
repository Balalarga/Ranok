#pragma once

#include <functional>

class GuiLayer
{
public:
	GuiLayer(std::function<void()> renderFunc);
	void Render();
	
	
private:
	std::function<void()> _renderFunc;
};
