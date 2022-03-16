#include <iostream>

#include "Graphics/Window.h"
#include "Graphics/Gui/Gui.h"
#include "Graphics/Opengl/Opengl.h"
#include "Graphics/Opengl/RayMarchingView.h"
#include "Utility/FileSystem.h"

#include <Ranok/LanguageCore/Parser.h>

using namespace std;


void MakeLayout(Window& window)
{
    Scene* scene = new Scene();
    RanokTextEditor* editor = new RanokTextEditor();
    RayMarchingView* rayMarchView = new RayMarchingView(scene);

    window.AddItem(new GuiItem([editor, rayMarchView](){
        ImGui::Begin("Tools");
        if (ImGui::Button("Open"))
        {
            string file = FileDialog::GetFilepath(FileDialog::FileMode::Open, "Text (*.TXT)\0");
            if (!file.empty())
            {
                CheckedResult<std::string> chackedFile = FileSystem::ReadSomeFile(file);
                editor->SetText(chackedFile.Get());
            }
        }
        if (ImGui::Button("Save as"))
        {
            string file = FileDialog::GetFilepath(FileDialog::FileMode::Save, "Text (*.TXT)\0");
            if (!file.empty())
            {
                CheckedResult<std::string> chackedFile = FileSystem::ReadSomeFile(file);
                editor->SetText(chackedFile.Get());
            }
        }
        if (ImGui::Button("Compile"))
        {
            Parser parser;
            Program program = parser.Parse(Lexer::CreateFrom(editor->Text()));
            rayMarchView->SetModel(program);
        }
        ImGui::End();
    }));

    window.AddItem(editor);

    scene->AddObject(rayMarchView);
    scene->SetBackgroundColor(ImVec4(0.1, 0.1, 0.1, 1));

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
