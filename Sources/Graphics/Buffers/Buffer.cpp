#include "Buffer.h"
#include <GL/glew.h>


DataPtr::DataPtr():
    Ptr(nullptr),
    Count(0),
    ItemSize(0)
{
    
}

DataPtr::DataPtr(void* ptr, unsigned count, unsigned itemSize):
    Ptr(ptr),
    Count(count),
    ItemSize(itemSize)
{

}
unsigned DrawType = GL_TRIANGLES;
unsigned Type = GL_ARRAY_BUFFER;
unsigned Mode = GL_STATIC_DRAW;

Buffer::Buffer(const DataPtr& data, const BufferLayout& layout):
    Data(data),
    Layout(layout),
    DrawType(GL_TRIANGLES),
    Type(GL_ARRAY_BUFFER),
    Mode(GL_STATIC_DRAW)
{

}

unsigned Buffer::Create()
{
    unsigned handler = 0;
    if (!Data.Ptr)
        return handler;
    
    glGenBuffers(1, &handler);
    glBindBuffer(Type, handler);
    glBufferData(Type, Data.Count * Data.ItemSize, Data.Ptr, Mode);

    unsigned offset = 0;
    for (int i = 0; i < Layout.Variables.size(); ++i)
    {
        glEnableVertexAttribArray(i);
        glVertexAttribPointer(i,
                              Layout.Variables[i].Count,
                              Layout.Variables[i].Type,
                              Layout.Variables[i].Normalized ? GL_FALSE : GL_TRUE,
                              Layout.Size,
                              (void*)offset);
        offset += Layout.Variables[i].Size * Layout.Variables[i].Count;
    }
    
    return handler;
}
