#include "FrameBuffer.h"

#include <GL/glew.h>

FrameBuffer::FrameBuffer(glm::uvec2 size, int channelsMode):
	_texture(size, channelsMode)
{

}

FrameBuffer::~FrameBuffer()
{
	glDeleteFramebuffers(1, &_fboId);
}

void FrameBuffer::Resize(glm::uvec2 size)
{
	glBindFramebuffer(GL_FRAMEBUFFER, _fboId);
    
	_texture.Resize(size);
    
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

unsigned FrameBuffer::Create()
{
	glGenFramebuffers(1, &_fboId);
	glBindFramebuffer(GL_FRAMEBUFFER, _fboId);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _texture.GetId(), 0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	return _fboId;
}

void FrameBuffer::RecreateTexture(glm::uvec2 size)
{
	_texture.Recreate(size);
	glBindFramebuffer(GL_FRAMEBUFFER, _fboId);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _texture.GetId(), 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBuffer::Bind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, _fboId);
}

void FrameBuffer::Release()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
