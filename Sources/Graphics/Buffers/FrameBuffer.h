#pragma once

#include "glm/glm.hpp"
#include "Graphics/Rendering/Texture.h"

class FrameBuffer
{
public:
	FrameBuffer(glm::uvec2 size);
	virtual ~FrameBuffer();

	void Resize(glm::uvec2 size);
    
	unsigned Create();

	virtual void Bind();
	void Release();

	unsigned GetTextureId() const { return _texture.GetId(); }

	glm::uvec2 GetTextureSize() { return _texture.Size(); }

private:
	Texture _texture;
	unsigned _fboId;
};
