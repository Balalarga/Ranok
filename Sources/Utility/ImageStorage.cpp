#include "ImageStorage.h"


ImageStorage &ImageStorage::Get()
{
    static ImageStorage self;
    return self;
}

ImageStorage::~ImageStorage()
{
    for (auto& i: _images)
        glDeleteTextures(1, &i.second.id);
}

ImageData ImageStorage::Load(const std::string &tag, const std::string &path, const std::string& root)
{
    auto image = GetImage(tag);
    if (image.Ok())
        return image.Get();

    ImageData data;
    SDL_Surface *surface;
    surface = IMG_Load((root + '/' + path).c_str());

    glGenTextures(1, &data.id);
    glBindTexture(GL_TEXTURE_2D, data.id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    data.width = surface->w;
    data.height = surface->h;

    SDL_FreeSurface(surface);

    _images[tag] = data;

    return data;
}

CheckedResult<ImageData> ImageStorage::GetImage(const std::string &tag)
{
    auto it = _images.find(tag);

    if (it == _images.end())
        return {};

    return {it->second};
}
