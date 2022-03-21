#include <iostream>

#include "Graphics/Window.h"
#include "Graphics/Gui/Gui.h"
#include "Editor/Editor.h"

using namespace std;


int main(int argc, char **argv)
{
    Window* window = Window::Create();
    if (!window)
    {
        cout << "Window creation error\n";
        return -1;
    }

    Editor editor;

    window->AddItem(&editor);

    window->Open();

    Window::Destroy();
    return 0;
}
