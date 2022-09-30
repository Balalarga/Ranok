#pragma once

#include "Editor/Modules/EditorModule.h"
#include "LoggerOutputDevice.h"
#include "Log/Logger.h"

namespace Ranok
{
class LoggerModule: public IEditorModule
{
public:
	LoggerModule(const std::string& inModuleName):
		IEditorModule(inModuleName)
	{
		Logger::AddOutputDevice(new LoggerOutputDevice(*this));
	}
	
	void RenderWindowContent() override
	{
		if (ImGui::Button("Clear"))
			Clear();
		ImGui::SameLine();
		bool copy = ImGui::Button("Copy");
		ImGui::Separator();
		ImGui::BeginChild("scrolling");
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0,1));
		if (copy)
			ImGui::LogToClipboard();
		ImGui::TextUnformatted(_textBuffer.begin());
		if (_bScrollToBottom)
			ImGui::SetScrollHereY(1.0f);
    
		_bScrollToBottom = false;
		ImGui::PopStyleVar();
		ImGui::EndChild();
	}

	void AddLog(const std::string& text)
	{
		_textBuffer.append(text.c_str());
		_textBuffer.append("\n");
		_bScrollToBottom = true;
	}
	
	void Clear()
	{
		_textBuffer.clear();
	}

	std::string OpenFileFilter() override
	{
		return "*.txt";
	}

protected:
	bool _bScrollToBottom = true;
	ImGuiTextBuffer _textBuffer;
};
}
