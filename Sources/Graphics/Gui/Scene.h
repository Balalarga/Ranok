#pragma once

#include <GL/glew.h>
#include <memory>

#include "GuiItem.h"
#include "Graphics/Opengl/Camera.h"
#include "Graphics/Opengl/Renderable.h"


class Scene: public GuiBase
{
public:
    Scene(ImVec2 renderSize = {800, 600}, const std::string &title = "Scene");
    ~Scene();

    void Render() override;

    template<class T, class ...TArgs>
    T* AddObject(TArgs ...args)
    {
        _objects.push_back(std::make_unique<T>(args...));
        return static_cast<T*>((_objects.back().get()));
    }
    bool DeleteObject(Renderable* ptr)
    {
        for (size_t i = 0; i < _objects.size(); ++i)
        {
            if (_objects[i].get() == ptr)
            {
                _objects.erase(_objects.begin() + i);
                return true;
            }
        }

        return false;
    }

    void SetBackgroundColor(const ImVec4& color);
    inline void NeedUpdate() { _needUpdate = true; }

    inline const ImVec2& GetRenderSize() { return _renderSize; }
    void SetRenderSize(unsigned x, unsigned y);

    const glm::mat4& GetCameraViewProject() { return _camera.GetViewMatrix(); }

    void HandleMouse(const ImVec2& mouseDelta);
    void HandleKeyboard(Camera::Camera_Movement dir, float deltaTime);
    void HandleScroll(float delta);

    inline Camera& GetCamera() { return _camera; }
    inline bool IsMouseHandle() { return _mouseHandle; }


protected:
    void UpdateTexture();


private:
    bool _needUpdate;
    unsigned _fbo;
    unsigned _texture;

    ImVec2 _renderSize;
    ImVec4 _backgroundColor;

    Camera _camera;
    bool _mouseHandle;

    std::vector<std::unique_ptr<Renderable>> _objects;
};
