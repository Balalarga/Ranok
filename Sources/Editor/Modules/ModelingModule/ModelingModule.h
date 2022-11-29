#pragma once
#include "RaymarchingView.h"
#include "TextEditor.h"
#include "Editor/Modules/EditorModule.h"
#include "Language/Generators/OpenclGenerator.h"

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
	
	bool TryOpenFile(const std::string& filepath);

	
protected:
	void RenderTextEditor();
	
	void CompileTab(int tabId);
	void BuildTab(int tabId);

	void OnTextChanged(TextEditorInfo& info);
	void UpdateViewport(ActionTree& tree);
	
private:
	std::vector<TextEditorInfo> _textEditorTabs;
	int currentActiveTab = -1;
	RayMarchingView _viewport;
	ImFont* _textEditorFont;
	
	OpenclGenerator _openclGenerator;
};
}
