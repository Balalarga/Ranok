#include "TextureManager.h"
#define STB_IMAGE_IMPLEMENTATION
#include "ThirdParty/stb_image.h"

namespace Ranok
{
// Simple helper function to load an image into a OpenGL texture with common settings
static bool LoadTextureFromFile(const char* filename, GLuint* out_texture, int* out_width, int* out_height)
{
	// Load from file
	int image_width = 0;
	int image_height = 0;
	unsigned char* image_data = stbi_load(filename, &image_width, &image_height, NULL, 4);
	if (image_data == NULL)
		return false;

	// Create a OpenGL texture identifier
	GLuint image_texture;
	glGenTextures(1, &image_texture);
	glBindTexture(GL_TEXTURE_2D, image_texture);

	// Setup filtering parameters for display
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // This is required on WebGL for non power-of-two textures
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // Same

	// Upload pixels into texture
	#if defined(GL_UNPACK_ROW_LENGTH) && !defined(__EMSCRIPTEN__)
	glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
#endif
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width, image_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
	stbi_image_free(image_data);

	*out_texture = image_texture;
	*out_width = image_width;
	*out_height = image_height;

	return true;
}

TextureManager& TextureManager::Instance()
{
	static TextureManager manager;
	return manager;
}

TextureManager::~TextureManager()
{
	for (auto& [tag, texture]: _textures)
		glDeleteTextures(1, &texture.id);
}

std::optional<TextureManager::TextureInfo> TextureManager::LoadTexture(const std::string& tag, const std::string& filepath)
{
	std::optional<TextureInfo> textureInfo = GetTextureInfo(tag);
	if (textureInfo.has_value())
		return textureInfo.value();

	TextureInfo info{0, {}, filepath};
	if (!LoadTextureFromFile(filepath.c_str(), &info.id, &info.size.x, &info.size.y))
		return std::nullopt;
	
	_textures[tag] = info;
	return info;
}

std::optional<GLuint> TextureManager::GetTexture(const std::string& tag)
{
	std::optional<TextureInfo> textureInfo = GetTextureInfo(tag);
	if (textureInfo.has_value())
		return textureInfo.value().id;
	
	return std::nullopt;
}

std::optional<TextureManager::TextureInfo> TextureManager::GetTextureInfo(const std::string& tag)
{
	auto it = _textures.find(tag);
	if (it != _textures.end())
		return it->second;
	
	return std::nullopt;
}
}
