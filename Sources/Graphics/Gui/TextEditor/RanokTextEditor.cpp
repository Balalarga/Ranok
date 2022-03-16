#include "RanokTextEditor.h"

#include <Ranok/LanguageCore/Functions.h>

#include "imgui_internal.h"
#include "Graphics/Gui/Dialogs/FileDialog.h"
#include "Utility/FileSystem.h"


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

//    ImGui::BeginMenuBar();
//    ImGui::BeginMenu("File");
//    if (ImGui::MenuItem("Open", "CTRL+O"))
//    {
//        std::string file = FileDialog::GetFilepath(FileDialog::FileMode::Open, "Text (*.TXT)\0");
//        if (!file.empty())
//        {
//            CheckedResult<std::string> chackedFile = FileSystem::ReadSomeFile(file);
//            SetText(chackedFile.Get());
//        }
//    }
//    if (ImGui::MenuItem("Save as", "CTRL+SHIFT+S"))
//    {
//        std::string file = FileDialog::GetFilepath(FileDialog::FileMode::Save, "Text (*.TXT)\0");
//        if (!file.empty())
//        {
//            CheckedResult<std::string> chackedFile = FileSystem::ReadSomeFile(file);
//            SetText(chackedFile.Get());
//        }
//    }
//    ImGui::EndMenu();
//    ImGui::EndMenuBar();
    ImGui::End();
}

void RanokTextEditor::OpenFile(const std::string &path)
{

}

void RanokTextEditor::SetText(const std::string &text)
{
    _editor->SetText(text);
}

std::string RanokTextEditor::Text()
{
    return _editor->GetText();
}

void RanokTextEditor::CreateLangDef()
{
    TextEditor::LanguageDefinition langDef = TextEditor::LanguageDefinition::CPlusPlus();

    static const char* const keywords[] = {
        "var", "arg", "variable", "argument", "arguments", "return"
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
//    langDef.mAutoIndentation = true;

    langDef.mName = "Ranok";

    _editor->SetLanguageDefinition(langDef);
}
