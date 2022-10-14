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
		for (const LogInfo& logInfo : _logs)
		{
			ImVec4 color = {1.0f, 1.0f, 1.0f, 1.0f};
			switch(logInfo.level)
			{
			case LogLevel::Log:
				break;
			case LogLevel::Warning:
				color = {1.0f, 0.5f, 0.0f, 1.0f};
				break;
			case LogLevel::Error:
				color = {1.0f, 0.1f, 0.0f, 1.0f};
				break;
			case LogLevel::Verbose:
				color = {0.3f, 1.f, 0.5f, 1.0f};
				break;
			}
			ImGui::TextColored(color, logInfo.text.c_str());
		}
		
		if (_bScrollToBottom)
			ImGui::SetScrollHereY(1.0f);
    
		_bScrollToBottom = false;
		ImGui::PopStyleVar();
		ImGui::EndChild();
	}

	void AddLog(LogLevel level, const std::string& text)
	{
		_logs.push_back({level, {text}});
		_bScrollToBottom = true;
	}
	
	void Clear()
	{
		_logs.clear();
	}

protected:
	struct LogInfo
	{
		LogLevel level;
		std::string text;
	};
	std::vector<LogInfo> _logs;
	bool _bScrollToBottom = true;
};
}
