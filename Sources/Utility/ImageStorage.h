#pragma once

#include <GL/glew.h>
#include <map>
#include <SDL_image.h>
#include "FileSystem.h"


struct ImageData
{
    GLuint id;
    unsigned width, height;
};


class ImageStorage
{
public:
    static ImageStorage& Get();
    ~ImageStorage();

    ImageData Load(const std::string& tag, const std::string& path, const std::string& root = FileSystem::GetAssetFolder());
    CheckedResult<ImageData> GetImage(const std::string& tag);


private:
    ImageStorage() = default;

    std::map<std::string, ImageData> _images;
};

