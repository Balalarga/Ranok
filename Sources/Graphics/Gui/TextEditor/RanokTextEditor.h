#pragma once

#include "Graphics/Gui/Base/GuiItem.h"
#include "TextEditor.h"


class RanokTextEditor: public TextEditor
{
public:
    RanokTextEditor();
    ~RanokTextEditor();


protected:
    void CreateLangDef();
};

