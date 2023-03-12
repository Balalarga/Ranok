#pragma once
#include "Graphics/Buffers/Buffer.h"

class IMaterial;

class Object
{
public:
	Object(Buffer vbo, IMaterial* material = nullptr);
	virtual ~Object();

	void SetMaterial(IMaterial* material);
	IMaterial* GetMaterial() { return _material; }
	
	virtual void Render() const;
	
private:
	unsigned _glHandler;
	
	Buffer _vbo;
	IMaterial* _material;
};
