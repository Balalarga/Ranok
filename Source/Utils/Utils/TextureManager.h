#pragma once

#include <map>
#include <optional>
#include <string>
#include <GL/glew.h>
#include <glm/vec2.hpp>

namespace Ranok
{
class TextureManager
{
public:
	struct TextureInfo
	{
		GLuint id;
		glm::ivec2 size;
		std::string filepath;
	};
	
	static TextureManager& Instance();
	~TextureManager();
	
	std::optional<TextureInfo> LoadTexture(const std::string& tag, const std::string& filepath);
	std::optional<GLuint> GetTexture(const std::string& tag);
	std::optional<TextureInfo> GetTextureInfo(const std::string& tag);
	
	
private:
	TextureManager() = default;
	
	std::map<std::string, TextureInfo> _textures;
};
}
