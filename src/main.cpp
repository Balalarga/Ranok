#include <iostream>
#include "Graphics/Window.h"

using namespace std;


void MakeLayout(Window& window)
{
    window.AddGuiItem(new GuiItem([](){
        ImGui::Begin("Window");
        ImGui::Button("Hello!");
        ImGui::End();
    }));

    float data[]{
        0, 0, 0, 1, 1, 1, 1,
        0, 0, 1, 1, 1, 1, 1,
        0, 1, 1, 1, 1, 1, 1,
    };

    window.AddRenderableObject(new Renderable(BufferInfo(data, 3)));
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
