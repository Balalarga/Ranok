#pragma once

#include "GuiItem.h"
#include "TextEditor.h"

class RanokTextEditor: public GuiItem
{
public:
    RanokTextEditor();
    ~RanokTextEditor();

    void Render();

protected:
    void CreateLangDef();

private:
    TextEditor* _editor;
};

