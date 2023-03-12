#pragma once
#include "RaymarchingView.h"
#include "TextEditor.h"
#include "../EditorModule.h"
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

	struct BuildTabParams
	{
		const int minDepth = 3; // Opencl local group size
		const int maxDepth = 15;
		const int filepathLen = 1024;
        const int batchMin = 20;

		BuildTabParams()
		{
			saveFilepath.resize(filepathLen);
		}

		std::string saveFilepath;
		int recursions = minDepth;
		bool batching = false;
		int batchSize = batchMin;
		bool openAfterBuild = false;
		std::vector<float> spaceCenter = {0, 0, 0};
		std::vector<float> spaceSize = {1, 1, 1};
	};
public:
	ModelingModule();
	
	void RenderWindowContent() override;
	void PostRender() override;
	
	bool TryOpenFile(const std::string& filepath);

	
protected:
	void RenderTextEditor();
	
	void CompileTab(int tabId);
	void BuildTab(int tabId, const BuildTabParams& params);

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
