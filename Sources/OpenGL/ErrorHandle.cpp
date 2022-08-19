#include "ErrorHandle.h"


void GLClearError()
{
    while(glGetError() != GL_NO_ERROR);
}

bool GLLogCall(const char* func, const char* file, int line)
{
    while(GLenum error = glGetError())
    {
        std::cout<<"[OpenglError] "<<line<<": "<<file<<" "<<func<<" - ";
        std::cout <<" (0x"<<std::hex<<error<<")\n";
        return false;
    }
    return true;
}