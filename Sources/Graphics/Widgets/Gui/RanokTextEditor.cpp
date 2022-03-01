#include "RanokTextEditor.h"
#include "Language/LangFunctions.h"
#include "Language/GeneratedFunctions.h"


RanokTextEditor::RanokTextEditor():
    GuiItem(std::bind(&RanokTextEditor::Render, this)),
    _editor(new TextEditor())
{
    _editor->SetShowWhitespaces(false);
    _editor->SetReadOnly(false);
    auto palette = TextEditor::GetDarkPalette();
    palette[(unsigned)TextEditor::PaletteIndex::Background] = 0xff2d2d2d;
    _editor->SetPalette(palette);

    CreateLangDef();
}

RanokTextEditor::~RanokTextEditor()
{
    delete _editor;
}

void RanokTextEditor::Render()
{
    ImGui::Begin("Editor");
    _editor->Render("Editor");
    ImGui::End();
}

void RanokTextEditor::CreateLangDef()
{
    TextEditor::LanguageDefinition langDef = TextEditor::LanguageDefinition::C();

    static const char* const cppKeywords[] = {
        "var", "arg", "variable", "argument", "arguments", "return", "constant", "const"
    };
    langDef.mKeywords.clear();
    for (auto& k : cppKeywords)
        langDef.mKeywords.insert(k);

    langDef.mIdentifiers.clear();
    for (auto& k : LangFunctions::functions)
    {
        TextEditor::Identifier id;
        id.mDeclaration = "Built-in function";
        langDef.mIdentifiers.insert(std::make_pair(k.first, id));
    }

    for (auto& k : GeneratedFunctions::OneArgFunctions)
    {
        TextEditor::Identifier id;
        id.mDeclaration = "Built-in function";
        langDef.mIdentifiers.insert(std::make_pair(k.first, id));
    }
    for (auto& k : GeneratedFunctions::TwoArgFunctions)
    {
        TextEditor::Identifier id;
        id.mDeclaration = "Built-in function";
        langDef.mIdentifiers.insert(std::make_pair(k.first, id));
    }
    for (auto& k : GeneratedFunctions::VoidFunctions)
    {
        TextEditor::Identifier id;
        id.mDeclaration = "Built-in function";
        langDef.mIdentifiers.insert(std::make_pair(k.first, id));
    }

    langDef.mCaseSensitive = false;

    _editor->SetLanguageDefinition(langDef);
}
