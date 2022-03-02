#pragma once

#include "Graphics/Gui/Base/GuiItem.h"
#include "TextEditor.h"


class RanokTextEditor: public GuiItem
{
public:
    RanokTextEditor();
    ~RanokTextEditor();

    void Render();
    void OpenFile(const std::string& path);
    void SetText(const std::string& text);


protected:
    void CreateLangDef();

private:
    TextEditor* _editor;
};

