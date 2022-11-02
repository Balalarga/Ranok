#include "ModelingModule.h"
#include "Executor/OpenclExecutor.h"
#include "Language/Parser.h"
#include "Localization/LocalizationSystem.h"
#include "Utils/FileUtils.h"
#include "Utils/WindowsUtils.h"

namespace Ranok
{
DEFINE_LOCTEXT(ModelingModuleName, "Modeling")
DEFINE_LOCTEXT(ModelingCloseTabTitle, "Close code")
DEFINE_LOCTEXT(ModelingCloseTabText, "Do you want to save code before closing?")
DEFINE_LOCTEXT(ModelingCloseTabSaveText, "Save")
DEFINE_LOCTEXT(ModelingCloseTabSaveAsText, "Save as...")
DEFINE_LOCTEXT(ModelingCloseTabCloseText, "Close")
DEFINE_LOCTEXT(ModelingCloseTabCancelText, "Cancel")
DEFINE_LOCTEXT(ModelingCompile, "Compile")
DEFINE_LOCTEXT(ModelingBuild, "Build")

ModelingModule::ModelingModule():
	IEditorModule(LOCTEXTSTR(ModelingModuleName), ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse),
	_viewport({800, 600})
{
	Opencl::Executor::Init();
	
	SetNoClosing(true);
	_viewport.Create();
	const ImGuiIO& io = ImGui::GetIO();
	const std::string fontPath = Files::GetAssetPath(_textEditorConfigs.textFont);
	_textEditorFont = io.Fonts->AddFontFromFileTTF(fontPath.c_str(),
		_textEditorConfigs.renderFontSize,
		nullptr,
		io.Fonts->GetGlyphRangesCyrillic());
	
	RenderViewport();
}

void ModelingModule::RenderWindowContent()
{
	static int currentActiveTab = -1;
	
	ImGui::BeginChild("mainContainer");
	static float w = ImGui::GetWindowContentRegionMax().x / 3.f;
	const float trueH = ImGui::GetWindowContentRegionMax().y;
	const float trueW = w >= 0 ? w : 1;
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
	ImGui::BeginChild("child1", ImVec2(trueW, trueH), true);
	if (!_textEditorTabs.empty())
	{
		ImGui::BeginTabBar("##textEditorTabs");
		for (size_t i = 0; i < _textEditorTabs.size(); ++i)
		{
			bool bOpen = true;
			if (_textEditorTabs[i].editor.IsTextChanged())
				OnTextChanged(_textEditorTabs[i]);
			
			ImGuiTabItemFlags flags = ImGuiTabItemFlags_None;
			if (_textEditorTabs[i].editor.CanUndo())
				flags |= ImGuiTabItemFlags_UnsavedDocument;
			
			if (ImGui::BeginTabItem(_textEditorTabs[i].filename.c_str(), &bOpen, flags))
			{
				currentActiveTab = static_cast<int>(i);
				if (ImGui::IsItemHovered())
				{
					ImGui::BeginTooltip();
					ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
					ImGui::TextUnformatted(_textEditorTabs[i].filepath.c_str());
					ImGui::PopTextWrapPos();
					ImGui::EndTooltip();
				}
				ImGui::SetWindowFontScale(_textEditorConfigs.fontSize / _textEditorConfigs.renderFontSize);
				ImGui::PushFont(_textEditorFont);
				_textEditorTabs[i].editor.Render(_textEditorTabs[i].filename.c_str());
				ImGui::PopFont();
				ImGui::EndTabItem();
				ImGui::SetWindowFontScale(1.f);
			}
			if (!bOpen)
			{
				if (_textEditorTabs[i].editor.CanUndo())
				{
					ImGui::OpenPopup(LOCTEXT(ModelingCloseTabTitle));
					break;
				}
				_textEditorTabs.erase(_textEditorTabs.begin() + currentActiveTab);
				--i;
			}
		}
		
		if (ImGui::BeginPopupModal(LOCTEXT(ModelingCloseTabTitle)))
		{
			ImGui::Text(LOCTEXT(ModelingCloseTabText));
			ImGui::Text("\n");
			ImGui::Separator();
			
			if (ImGui::Button(LOCTEXT(ModelingCloseTabSaveText)))
			{
				TextEditorInfo& currentTab = _textEditorTabs[currentActiveTab];
				if (!currentTab.filepath.empty() &&
					Files::WriteToFile(currentTab.filepath, currentTab.editor.GetText()))
				{
					Logger::Log(fmt::format("RCode saved to {}", currentTab.filepath));
					_textEditorTabs.erase(_textEditorTabs.begin() + currentActiveTab);
					ImGui::CloseCurrentPopup();
				}
			}
			
			ImGui::SameLine();
			if (ImGui::Button(LOCTEXT(ModelingCloseTabSaveAsText)))
			{
				std::string toSave = SaveFileDialog();
				if (!toSave.empty() && Files::WriteToFile(toSave, _textEditorTabs[currentActiveTab].editor.GetText()))
				{
					_textEditorTabs.erase(_textEditorTabs.begin() + currentActiveTab);
					Logger::Log(fmt::format("RCode saved to {}", toSave));
					ImGui::CloseCurrentPopup();
				}
			}
			
			ImGui::SameLine();
			if (ImGui::Button(LOCTEXT(ModelingCloseTabCloseText)))
			{
				ImGui::CloseCurrentPopup();
				_textEditorTabs.erase(_textEditorTabs.begin() + currentActiveTab);
			}
			
			ImGui::SameLine();
			if (ImGui::Button(LOCTEXT(ModelingCloseTabCancelText)))
				ImGui::CloseCurrentPopup();
			
			ImGui::EndPopup();
		}
		ImGui::EndTabBar();
	}
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(5, 10));
	ImGui::BeginGroup();
	if (ImGui::Button(LOCTEXT(ModelingCompile)))
	{
		CompileTab(currentActiveTab);
	}
	ImGui::SameLine();
	if (ImGui::Button(LOCTEXT(ModelingBuild)))
	{
		BuildTab(currentActiveTab);
	}
	ImGui::PopStyleVar();
	ImGui::EndGroup();
	ImGui::EndChild();
	
	ImGui::SameLine();
	ImGui::InvisibleButton("vSplitter", ImVec2(8.0f, trueH));
	const bool bIsActive = ImGui::IsItemActive();
	const bool bIsHovered = ImGui::IsItemHovered();
	if (bIsActive)
		w += ImGui::GetIO().MouseDelta.x;
	if (bIsActive || bIsHovered)
		ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);
	ImGui::SameLine();
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::BeginChild("child2", ImVec2(0, trueH), true);
	ImGui::Image((void*)(intptr_t)_viewport.GetTextureId(),
				 ImGui::GetWindowSize(),
				 ImVec2(0, 1),
				 ImVec2(1, 0));
	ImGui::EndChild();
	ImGui::PopStyleVar(2);
	ImGui::EndChild();
}

void ModelingModule::PostRender()
{
}

bool ModelingModule::TryOpenFile(const std::string& filepath)
{
	if (!filepath.ends_with(".rcode"))
		return false;
	
	std::optional<std::string> data = Files::ReadFile(filepath);
	if (!data.has_value())
		return false;

	size_t nameStart = filepath.find_last_of('\\')+1;
	auto existsItem = std::ranges::find_if(_textEditorTabs,
	                                       [&filepath, &nameStart](const TextEditorInfo& info)
	                                       {
		                                       return info.filename == filepath.substr(nameStart);
	                                       });
	if (existsItem != _textEditorTabs.end())
	{
		Logger::Log(fmt::format("RCode {} already opened", filepath));
		return true;
	}
	
	TextEditorInfo& lastItem = _textEditorTabs.emplace_back();
	lastItem.editor.SetText(data.value());
	lastItem.editor.SetLanguageDefinition(TextEditor::LanguageDefinition::RanokLanguageDefinition());
	lastItem.filename = filepath.substr(nameStart);
	lastItem.filepath = filepath;
	Logger::Log(fmt::format("RCode {} opened", filepath));
	return true;
}

void ModelingModule::CompileTab(int tabId)
{
	if (tabId < 0 || tabId >= _textEditorTabs.size())
		return;

	TextEditorInfo& currentTab = _textEditorTabs[tabId];
	std::string text = currentTab.editor.GetText();

	Lexer lexer(text);
	Parser parser;
	ActionTree tree = parser.Parse(lexer);
	if (parser.HasErrors())
	{
		for (const std::string& error : parser.Errors())
			Logger::Error(error);
		return;
	}
	
	if (!tree.Root())
	{
		Logger::Error("No main function founded");
		return;
	}
	UpdateViewport(tree);
	
	Logger::Log("Success");
}

void ModelingModule::BuildTab(int tabId)
{
	if (tabId < 0 || tabId >= _textEditorTabs.size())
		return;

	TextEditorInfo& currentTab = _textEditorTabs[tabId];
	std::string text = currentTab.editor.GetText();

	Lexer lexer(text);
	Parser parser;
	ActionTree tree = parser.Parse(lexer);
	if (parser.HasErrors())
	{
		for (const std::string& error : parser.Errors())
			Logger::Error(error);
		return;
	}
	
	if (!tree.Root())
	{
		Logger::Error("No main function founded");
		return;
	}
	
	std::optional<std::string> res = _openclGenerator.Generate(tree);
	if (!res)
	{
		Logger::Error("Generation failed");
		return;
	}
	
	Opencl::Executor exec;
	int result = exec.Compile(res.value());
	if (result != CL_SUCCESS)
	{
		Logger::Error(fmt::format("OpenCL compilation error: {}", result));
		return;
	}
	
	Logger::Log("Success");
}

void ModelingModule::OnTextChanged(TextEditorInfo& info)
{
	Lexer lexer(info.editor.GetText());
	Parser parser;
	ActionTree tree = parser.Parse(lexer);
	for (std::pair<const std::string, FunctionDeclarationNode*>& func : tree.GlobalFactory().Functions())
	{
		TextEditor::Identifier id;
		id.mDeclaration = FunctionDeclarationNode::GetDescription(func.second);
		info.editor.EditLanguageDefinition().mIdentifiers.insert(std::make_pair(func.first, id));
	}
}

void ModelingModule::UpdateViewport(ActionTree& tree)
{
	std::optional<std::string> code = _viewport.SetProgram(tree);
	if (!code.has_value())
	{
		Logger::Error("Shader compilation error");
		return;
	}
	
	Logger::Verbose(fmt::format("{}", code.value()));
	RenderViewport();
}

void ModelingModule::RenderViewport()
{
	_viewport.Bind();
	_viewport.Render();
	_viewport.Release();
}
}
