#pragma once
#include <string>

enum class ShaderType: uint8_t
{
	Vertex = 0,
	Fragment,
	Geometry,
	Count
};

class ShaderPart
{
public:
	ShaderPart() = default;
	ShaderPart(ShaderType type, const std::string& code);
	virtual ~ShaderPart();
	
	void Setup(ShaderType type, const std::string& code);
	
	bool IsInit() const { return _glHandler != 0; }
	
	bool Compile();
	void Destroy();
	
	const ShaderType& GetType() const { return _type; }
	unsigned GetGLType() const { return GetGLType(_type); }
	const std::string& GetCode() const { return _code; }
	const unsigned& GetHandler() const { return _glHandler; }


protected:
	static unsigned GetGLType(ShaderType type);

	bool HasError();


private:
	ShaderType _type = ShaderType::Vertex;
	std::string _code;
	
	unsigned _glHandler = 0;
};
