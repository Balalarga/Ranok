#include "GuiLayer.h"

GuiLayer::GuiLayer(std::function<void()> renderFunc):
	_renderFunc(std::move(renderFunc))
{
}

void GuiLayer::Render()
{
	if (_renderFunc)
		_renderFunc();	
}
