#pragma once
#include "TextEditor.h"

#include "Editor/Modules/EditorModule.h"

#include "OpenGL/Core/FrameBuffer.h"

namespace Ranok
{
class ModelingModule: public IEditorModule
{
public:
	ModelingModule();
	
	void RenderWindowContent() override;
	void PostRender() override;
	
	bool TryOpenFile(const std::string& filepath) override;
	
private:
	std::vector<TextEditor> _textEditorTabs;
	FrameBuffer _viewport;
};
}
