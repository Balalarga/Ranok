#pragma once

#include <GL/glew.h>
#include <vector>


struct VariableInfo
{
    unsigned Type;
    unsigned Count;
    unsigned Size;
    bool Normalized = true;
};


struct BufferLayout
{
    template<class T> BufferLayout& Add(unsigned count) {}
    template<float> BufferLayout& Add(unsigned count) { return Float(count); }
    template<int> BufferLayout& Add(unsigned count) { return Int(count); }
    template<unsigned> BufferLayout& Add(unsigned count) { return Unsigned(count); }
    BufferLayout& Float(unsigned count);
    BufferLayout& Int(unsigned count);
    BufferLayout& Unsigned(unsigned count);
    
    std::vector<VariableInfo> Variables;
    unsigned Size = 0;

};
