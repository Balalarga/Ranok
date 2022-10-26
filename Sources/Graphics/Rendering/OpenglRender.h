#pragma once
#include <memory>
#include <vector>
#include <SDL_video.h>
#include "IRender.h"

#include "Object.h"

class OpenglRender: public IRender
{
public:
	OpenglRender(SDL_Window* window);
	
	void Clear() override;
	void Render() override;
	void Present() override;

	void SetClearColor(const SDL_Color& color);

	template<class T, class ...TArgs>
	std::enable_if_t<std::derived_from<T, Object>, std::shared_ptr<T>> CreateObject(TArgs&& ...args)
	{
		std::shared_ptr<Object> obj = std::make_shared<T>(std::forward<TArgs>(args)...);
		_objects.push_back(obj);
		return obj;
	}
	
private:
	SDL_Window* _window;
	SDL_Color _backColor{60, 60, 60, 255};

	std::vector<std::shared_ptr<Object>> _objects;
};
