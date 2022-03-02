#include <iostream>

#include "Graphics/Window.h"
#include "Graphics/Gui/Gui.h"
#include "Graphics/Opengl/Opengl.h"
#include "Utility/FileSystem.h"

using namespace std;

void MakeLayout(Window& window)
{
    auto* editor = new RanokTextEditor();

    window.AddItem(new GuiItem([editor](){
        ImGui::Begin("Tools");
        if (ImGui::Button("Open"))
        {
            string file = FileDialog::GetFilepath("Text (*.TXT)\0");
            if (!file.empty())
                editor->SetText(FileSystem::ReadSomeFile(file));
        }
        ImGui::End();
    }));

    window.AddItem(editor);

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
