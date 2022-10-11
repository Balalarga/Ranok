#pragma once
#include "TextEditor.h"
#include "TextEditorSettings.h"
#include "Editor/Modules/EditorModule.h"
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
	
private:
	struct TextEditorInfo
	{
		std::string filename;
		std::string filepath;
		TextEditor editor;
	};
	TextEditorSettings _textEditorSettings;
	std::vector<TextEditorInfo> _textEditorTabs;
	FrameBuffer _viewport;
	ImFont* _textEditorFont;
};
}
