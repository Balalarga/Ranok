#include "Buffer.h"

#include "OpenGL/ErrorHandle.h"


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

Buffer::Buffer(const DataPtr& data, const BufferLayout& layout):
    Data(data),
    Layout(layout)
{

}

unsigned Buffer::Create()
{
    unsigned handler = 0;
    if (!Data.Ptr)
        return handler;
    
    GLCall(glGenBuffers(1, &handler))
    GLCall(glBindBuffer(Type, handler))
    GLCall(glBufferData(Type, Data.Count * Data.ItemSize, Data.Ptr, Mode))

    unsigned offset = 0;
    for (int i = 0; i < Layout.Variables.size(); ++i)
    {
        GLCall(glEnableVertexAttribArray(i))
        GLCall(glVertexAttribPointer(i,
                              Layout.Variables[i].Count,
                              Layout.Variables[i].Type,
                              Layout.Variables[i].Normalized ? GL_FALSE : GL_TRUE,
                              Layout.Size,
                              static_cast<void*>(&offset)))
        offset += Layout.Variables[i].Size * Layout.Variables[i].Count;
    }
    
    return handler;
}
