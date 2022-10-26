#pragma once
#include <vector>

class BufferLayout
{
public:
	struct VariableInfo
	{
		unsigned Type;
		unsigned Count;
		unsigned Size;
		bool Normalized = true;
	};
	
	BufferLayout& Float(unsigned count);
	BufferLayout& Int(unsigned count);
	BufferLayout& Unsigned(unsigned count);
    
	std::vector<VariableInfo> Variables;
	unsigned Size = 0;	
};
