#include "OpenglRender.h"

#include <cassert>
#include <GL/glew.h>

OpenglRender::OpenglRender(SDL_Window* window):
	_window(window)
{
	assert(window);
}

void OpenglRender::Clear()
{
	glClearColor(_backColor.r/255.f, _backColor.g/255.f, _backColor.b/255.f, _backColor.a/255.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void OpenglRender::Render()
{
	for (const std::shared_ptr<Object>& object : _objects)
	{
		object->Render();
	}
}

void OpenglRender::Present()
{
	SDL_GL_SwapWindow(_window);
}

void OpenglRender::SetClearColor(const SDL_Color& color)
{
	_backColor = color;
}
