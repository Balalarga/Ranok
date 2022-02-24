#include <iostream>
#include "Graphics/Window.h"

using namespace std;


int main(int argc, char **argv)
{
    Window* window = Window::Create();
    if (!window)
    {
        cout << "Window not created\n";
        return -1;
    }

    window->Open();

    Window::Destroy();
    return 0;
}
