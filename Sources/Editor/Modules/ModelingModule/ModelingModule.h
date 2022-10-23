#pragma once
#include "TextEditor.h"
#include "TextEditorSettings.h"
#include "Editor/Modules/EditorModule.h"
#include "Language/Generators/OpenclGenerator.h"
#include "OpenGL/Core/FrameBuffer.h"

namespace Ranok
{
class ModelingModule: public IEditorModule
{
	struct TextEditorTab
	{
		std::string filepath;
		std::string filename;
		TextEditor editor;
		void Render()
		{
			editor.Render(filename.c_str());
		}
	};
public:
	ModelingModule();
	
	void RenderWindowContent() override;
	void PostRender() override;
	
	bool TryOpenFile(const std::string& filepath) override;

	
protected:
	void CompileTab(int tabId);
	void BuildTab(int tabId);
	
	
private:
	struct TextEditorInfo
	{
		std::string filename;
		std::string filepath;
		TextEditor editor;
	};
	TextEditorconfigs _textEditorconfigs;
	std::vector<TextEditorInfo> _textEditorTabs;
	FrameBuffer _viewport;
	ImFont* _textEditorFont;
	
	OpenclGenerator _openclGenerator;
};
}
