#include "RanokTextEditor.h"
#include "Language/LangFunctions.h"
#include "Language/GeneratedFunctions.h"


RanokTextEditor::RanokTextEditor():
    GuiItem(std::bind(&RanokTextEditor::Render, this)),
    _editor(new TextEditor())
{
    _editor->SetShowWhitespaces(false);
    _editor->SetReadOnly(false);
//    auto palette = TextEditor::GetDarkPalette();
//    palette[(unsigned)TextEditor::PaletteIndex::Background] = 0xff2d2d2d;
//    _editor->SetPalette(palette);

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

void RanokTextEditor::OpenFile(const std::string &path)
{

}

void RanokTextEditor::SetText(const std::string &text)
{
    _editor->SetText(text);
}

void RanokTextEditor::CreateLangDef()
{
    TextEditor::LanguageDefinition langDef;

    static const char* const cppKeywords[] = {
        "var", "arg", "variable", "argument", "arguments", "return", "constant", "const"
    };
    for (auto& k : cppKeywords)
        langDef.mKeywords.insert(k);

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

    langDef.mTokenRegexStrings.push_back(std::make_pair<std::string, TextEditor::PaletteIndex>("[ \\t]*#[ \\t]*[a-zA-Z_]+", TextEditor::PaletteIndex::Preprocessor));
    langDef.mTokenRegexStrings.push_back(std::make_pair<std::string, TextEditor::PaletteIndex>("L?\\\"(\\\\.|[^\\\"])*\\\"", TextEditor::PaletteIndex::String));
    langDef.mTokenRegexStrings.push_back(std::make_pair<std::string, TextEditor::PaletteIndex>("\\'\\\\?[^\\']\\'", TextEditor::PaletteIndex::CharLiteral));
    langDef.mTokenRegexStrings.push_back(std::make_pair<std::string, TextEditor::PaletteIndex>("[+-]?([0-9]+([.][0-9]*)?|[.][0-9]+)([eE][+-]?[0-9]+)?[fF]?", TextEditor::PaletteIndex::Number));
    langDef.mTokenRegexStrings.push_back(std::make_pair<std::string, TextEditor::PaletteIndex>("[+-]?[0-9]+[Uu]?[lL]?[lL]?", TextEditor::PaletteIndex::Number));
    langDef.mTokenRegexStrings.push_back(std::make_pair<std::string, TextEditor::PaletteIndex>("0[0-7]+[Uu]?[lL]?[lL]?", TextEditor::PaletteIndex::Number));
    langDef.mTokenRegexStrings.push_back(std::make_pair<std::string, TextEditor::PaletteIndex>("0[xX][0-9a-fA-F]+[uU]?[lL]?[lL]?", TextEditor::PaletteIndex::Number));
    langDef.mTokenRegexStrings.push_back(std::make_pair<std::string, TextEditor::PaletteIndex>("[a-zA-Z_][a-zA-Z0-9_]*", TextEditor::PaletteIndex::Identifier));
    langDef.mTokenRegexStrings.push_back(std::make_pair<std::string, TextEditor::PaletteIndex>("[\\[\\]\\{\\}\\!\\%\\^\\&\\*\\(\\)\\-\\+\\=\\~\\|\\<\\>\\?\\/\\;\\,\\.]", TextEditor::PaletteIndex::Punctuation));

    langDef.mSingleLineComment = "//";

    langDef.mCaseSensitive = true;
    langDef.mAutoIndentation = true;

    langDef.mName = "Ranok";

    _editor->SetLanguageDefinition(langDef);
}
