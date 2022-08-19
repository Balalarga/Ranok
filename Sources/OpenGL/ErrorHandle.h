#pragma once

#include <GL/glew.h>
#include <iostream>

void GLClearError();
bool GLLogCall(const char* func, const char* file, int line);

/* Error handling */
#define DEBUG_ASSERT(x) if(!(x)) __debugbreak();

/* Use GLCall wrapper for handling opengl function errors */
#define GLCall(x) GLClearError(); \
    x;\
    DEBUG_ASSERT(GLLogCall(#x, __FILE__, __LINE__))

