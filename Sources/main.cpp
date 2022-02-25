#include <iostream>
#include "Graphics/Window.h"
#include "Graphics/Scene.h"

using namespace std;


void MakeLayout(Window& window)
{
    window.AddItem(new GuiItem([](){
        ImGui::Begin("Window");
        ImGui::Button("Hello!");
        ImGui::End();
    }));


    Scene* scene = new Scene();


    struct Vertex
    {
        float x, y, z;
        float r, g, b, a;
    };

    Vertex data[]
    {
        {0, 0, 0, 1, 1, 1, 1},
        {0, 1, 0, 1, 1, 1, 1},
        {1, 1, 0, 1, 1, 1, 1},
    };
    unsigned vertexCount = sizeof(data)/sizeof(data[0]);
    BufferInfo bufferInfo(data, vertexCount);
    scene->AddObject(new Renderable(bufferInfo));

    window.AddItem(scene);
}

int main(int argc, char **argv)
{
    Window* window = Window::Create();
    if (!window)
    {
        cout << "Window creation error\n";
        return -1;
    }
    MakeLayout(*window);

    window->Open();

    Window::Destroy();
    return 0;
}
