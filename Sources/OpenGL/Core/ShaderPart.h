#pragma once
#include <string>


class ShaderPart
{
public:
	enum class Type
	{
		Vertex, Fragment, Geometry
	};

	ShaderPart() = default;
	ShaderPart(Type type, const std::string& code);
	virtual ~ShaderPart();
	
	void Setup(Type type, const std::string& code);
	
	bool IsInited() const { return _handler != 0; }
	
	bool Compile();
	void Destroy();
	
	const Type& GetType() const { return _type; }
	unsigned GetGLType() const { return GetGLType(_type); }
	const std::string& GetCode() const { return _code; }
	const unsigned& GetHandler() const { return _handler; }


protected:
	static unsigned GetGLType(Type type);

	bool HasError();


private:
	Type _type = Type::Vertex;
	std::string _code;
	
	unsigned _handler = 0;
};
