#pragma once
#include "RaymarchingView.h"
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
	
	struct TextEditorInfo
	{
		std::string filename;
		std::string filepath;
		TextEditor editor;
	};
public:
	ModelingModule();
	
	void RenderWindowContent() override;
	void PostRender() override;
	
	bool TryOpenFile(const std::string& filepath) override;

	
protected:
	void CompileTab(int tabId);
	void BuildTab(int tabId);

	void OnTextChanged(TextEditorInfo& info);
	void UpdateViewport();
	
private:
	TextEditorConfigs _textEditorConfigs;
	std::vector<TextEditorInfo> _textEditorTabs;
	FrameBuffer _viewport;
	RayMarchingView _rayMarchView;
	ImFont* _textEditorFont;
	
	OpenclGenerator _openclGenerator;
};
}
