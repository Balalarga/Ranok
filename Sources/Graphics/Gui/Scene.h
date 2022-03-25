#pragma once

#include <GL/glew.h>
#include <memory>

#include "GuiItem.h"
#include "Graphics/Opengl/Camera.h"

class Renderable;


class Scene: public GuiBase
{
public:
    Scene(ImVec2 renderSize = {800, 600}, const std::string &title = "Scene");
    ~Scene();

    void Render() override;

    template<class T, class ...TArgs>
    T& AddObject(TArgs ...args)
    {
        _objects.push_back(std::make_unique<T>(args...));
        return *static_cast<T*>((_objects.back().get()));
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


protected:
    void UpdateTexture();


private:
    bool _needUpdate;
    unsigned _fbo;
    unsigned _texture;

    ImVec2 _renderSize;
    ImVec4 _backgroundColor;

    Camera _camera;

    std::vector<std::unique_ptr<Renderable>> _objects;
};
