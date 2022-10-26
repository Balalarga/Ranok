#pragma once
#include "Graphics/Shading/Shader.h"

class Shader;

class IMaterial
{
public:
	IMaterial(std::shared_ptr<Shader> shader);
	virtual ~IMaterial() = default;
	
	void Bind() const { _shader->Bind(); }
	static void Release() { Shader::Release(); }

	/// Calls between \b Binding and \b Rendering of \b Object
	virtual void SetupUniforms() = 0;

	std::weak_ptr<Shader> GetShader() { return _shader; }
	void SetShader(std::shared_ptr<Shader> shader);
	
protected:
    void SetUniform(const std::string& name, const UniformValue& value, bool bUnbind = false) const
    {
    	_shader->SetUniform(name, value, bUnbind);
    }
	
private:
	std::shared_ptr<Shader> _shader;
};
