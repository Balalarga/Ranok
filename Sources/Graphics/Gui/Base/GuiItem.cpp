#include "GuiItem.h"


void GuiBase::Render()
{
    PreRender();
    OnRender();
    PostRender();
}


GuiItemRaw::GuiItemRaw(const std::function<void ()> &renderFunc):
    _renderFunc(renderFunc)
{

}

GuiWindow::GuiWindow(const std::string &title):
    _title(title),
    _windowFlags(ImGuiWindowFlags_None)
{

}

GuiWindow::~GuiWindow()
{
    for (auto& child: _childs)
        delete child;
}

void GuiWindow::PreRender()
{
    ImGui::SetNextWindowSize(Size());
    ImGui::SetNextWindowPos(Pos());
    ImGui::Begin(_title.c_str(), NULL, _windowFlags);
}

void GuiWindow::OnRender()
{
    for (auto& i: _childs)
        i->Render();
}

void GuiWindow::PostRender()
{
    ImGui::End();
}

void GuiWindow::AddWindowFlags(int var)
{
    _windowFlags |= var;
}

void GuiWindow::RemoveWindowFlags(int var)
{
    _windowFlags &= var;
}

void GuiItemRaw::OnRender()
{
    _renderFunc();
}

GuiChildWindow::GuiChildWindow(const std::string &name):
    GuiWindow(name),
    _border(false)
{

}

void GuiChildWindow::PreRender()
{
    ImGui::BeginChild(_name.c_str(), Size(), _border, WindowFlags());
}

void GuiChildWindow::PostRender()
{
    ImGui::EndChild();
}
