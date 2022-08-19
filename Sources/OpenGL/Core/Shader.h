#pragma once

#include <map>
#include <set>
#include <string>
#include <vector>
#include <GL/glew.h>
#include <glm/glm.hpp>

#include "ShaderPart.h"


class IRenderable;

class Shader
{
public:
    Shader(ShaderPart* vShader, ShaderPart* fShader, ShaderPart* gShader = nullptr);
    virtual ~Shader();

	void AttachTo(IRenderable* object);
	void DetachFrom(IRenderable* object);
	
	bool IsInited() const { return _handler != 0; }

	bool Compile(bool bCompileParts = false);
    void Destroy();

	void BatchRender();
	
    void Bind() const;
    void Unbind() const;
	
	bool AddUniform(const std::string& name);
	bool AddUniforms(const std::vector<std::string>& names);

    void SetUniform(const std::string& name, const int& value);
    void SetUniform(const std::string& name, const float& value);
    void SetUniform(const std::string& name, const glm::vec2& value);
    void SetUniform(const std::string& name, const glm::vec3& value);
    void SetUniform(const std::string& name, const glm::vec4& value);
    void SetUniform(const std::string& name, const glm::mat4& value);
    void SetUniform(const std::string& name, const glm::mat3& value);


protected:
    int GetUniformLoc(const std::string& name);
    
    bool HasErrors(unsigned shaderId);
	

private:
	struct {
		ShaderPart* vShader{};
		ShaderPart* fShader{};
		ShaderPart* gShader{};
	} _parts;
	std::set<IRenderable*> _renderableObjects;
	
    std::map<std::string, int> _uniforms;

    GLuint _handler = 0;
};

