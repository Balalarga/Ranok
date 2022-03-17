#include <iostream>

#include "Graphics/Window.h"
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
    window->AddItem(new Editor);

    window->Open();

    Window::Destroy();
    return 0;
}
