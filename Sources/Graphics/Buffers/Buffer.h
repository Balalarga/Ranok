#pragma once

#include "BufferLayout.h"


struct DataPtr
{
    DataPtr();
    DataPtr(void* ptr, unsigned count, unsigned itemSize);
    
    template<class T>
    DataPtr(const std::vector<T>& items):
        DataPtr((void*)&items[0], items.size(), sizeof(T))
    {}
    template<class T>
    DataPtr(const std::initializer_list<T>& items):
        DataPtr((void*)&items[0], items.size(), sizeof(T))
    {}

    void* Ptr;
    unsigned Count;
    unsigned ItemSize;
};

struct Buffer
{
    Buffer(const DataPtr& data = {}, const BufferLayout& layout = BufferLayout());

    DataPtr Data;
    BufferLayout Layout;
    unsigned DrawType;
    unsigned Type;
    unsigned Mode;

    unsigned Create();
};
