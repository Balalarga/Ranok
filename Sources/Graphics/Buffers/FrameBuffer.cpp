#include "FrameBuffer.h"

#include <GL/glew.h>

FrameBuffer::FrameBuffer(glm::uvec2 size):
	_texture(size)
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

	_texture.Create();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	return _fboId;
}

void FrameBuffer::Bind()
{
	glBindFramebuffer(GL_FRAMEBUFFER, _fboId);
}

void FrameBuffer::Release()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
