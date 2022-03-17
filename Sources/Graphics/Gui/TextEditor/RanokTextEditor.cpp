#include "RanokTextEditor.h"

#include <Ranok/LanguageCore/Functions.h>

#include "imgui_internal.h"
#include "Graphics/Gui/Dialogs/FileDialog.h"
#include "Utility/FileSystem.h"


RanokTextEditor::RanokTextEditor():
    TextEditor("RanokTextEditor##ranokTextEditor1")
{
    SetShowWhitespaces(true);
    SetReadOnly(false);
    CreateLangDef();
}

RanokTextEditor::~RanokTextEditor()
{

}

void RanokTextEditor::CreateLangDef()
{
    TextEditor::LanguageDefinition langDef = TextEditor::LanguageDefinition::CPlusPlus();

    static const char* const keywords[] = {
        "var", "args", "variable", "argument", "arguments", "return"
    };
    for (auto& k : keywords)
        langDef.mKeywords.insert(k);

    // {declaration, name}
    std::vector<std::pair<std::string, std::string>> identifiers;

    for (auto& f: Functions::GetAll())
        identifiers.push_back({"Build-in function", f.name});

    for (auto& f: Functions::GetAllCustoms())
        identifiers.push_back({"User-define function", f->Info().name});


    for (auto& k : identifiers)
    {
        TextEditor::Identifier id;
        id.mDeclaration = k.first;
        langDef.mIdentifiers.insert(std::make_pair(k.second, id));
    }

    langDef.mCaseSensitive = false;

    langDef.mName = "Ranok";

    SetLanguageDefinition(langDef);
}
