#include "FrameBuffer.h"


namespace Ranok
{
FrameBuffer::FrameBuffer(glm::uvec2 size):
    _texture(size)
{

}

FrameBuffer::~FrameBuffer()
{
    glDeleteFramebuffers(1, &_fboId);
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
}
