#include "ModelingModule.h"

#include "Config/ConfigManager.h"
#include "Config/IConfig.h"
#include "Executor/OpenclExecutor.h"
#include "Language/Parser.h"
#include "Localization/LocalizationSystem.h"
#include "VoxelCore/Space/MultiDimSpace.h"
#include "Utils/FileUtils.h"
#include "Utils/FlatArray.h"
#include "Utils/WindowsUtils.h"
#include "Utils/Archives/Json/JsonArchive.h"

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
DEFINE_LOCTEXT(ModelingOpenFile, "Open")


namespace ModelingModule_Local
{
static FlatArray<MImage3D> imageBuffer;
}


class ModelingModuleConfig: public IConfig
{
public:
	ModelingModuleConfig(): IConfig("Modules/ModelingConfig")
	{
	}
	
	void Serialize(JsonArchive& archive) override
	{
		archive.Serialize("textEditorWidth", textEditorWidth);
		archive.Serialize("fontSize", fontSize);
		archive.Serialize("textFont", textFont);
		archive.Serialize("renderFontSize", renderFontSize);
	}
	bool operator!=(const IConfig& oth) override
	{
		const auto casted = dynamic_cast<const ModelingModuleConfig*>(&oth);
        assert(casted);
		return
			casted->textEditorWidth != textEditorWidth ||
			casted->fontSize != fontSize ||
			casted->renderFontSize != renderFontSize ||
			casted->textFont != textFont;
	}
	
	float textEditorWidth = -1;
	std::vector<std::string> openedFiles;
	
	std::string textFont = "Fonts/UbuntuMono/UbuntuMono-Regular.ttf";
	float fontSize = 16.f;
	float renderFontSize = 50.f;
};

static std::shared_ptr<ModelingModuleConfig> config = ConfigManager::Instance().CreateConfigs<ModelingModuleConfig>();

ModelingModule::ModelingModule():
	IEditorModule(LOCTEXTSTR(ModelingModuleName), ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse)
{
	Opencl::Executor::Init();
	
	SetNoClosing(true);
	_viewport.Create();
	const ImGuiIO& io = ImGui::GetIO();
	const std::string fontPath = Files::GetAssetPath(config->textFont);
	_textEditorFont = io.Fonts->AddFontFromFileTTF(fontPath.c_str(),
		config->renderFontSize,
		nullptr,
		io.Fonts->GetGlyphRangesCyrillic());
	
	_viewport.UpdateImage();
}

void ModelingModule::RenderWindowContent()
{
	if (config->textEditorWidth < 1)
		config->textEditorWidth = ImGui::GetWindowContentRegionMax().x/3.f;
	
	ImGui::BeginGroup();
		if (ImGui::Button(LOCTEXT(ModelingOpenFile)))
		{
			const std::string filepathStr = OpenFileDialog("*.rcode\0\0");
			if (!filepathStr.empty())
				TryOpenFile(filepathStr);
		}
		ImGui::SameLine();
		if (ImGui::Button(LOCTEXT(ModelingCompile)))
			CompileTab(currentActiveTab);
		ImGui::SameLine();
		if (ImGui::Button(LOCTEXT(ModelingBuild)))
			ImGui::OpenPopup("BuildTabParamsSelection");
	ImGui::EndGroup();
	
	ImGui::BeginChild("WorkingChild");
		ImGui::BeginChild("TextEditorChild", ImVec2(config->textEditorWidth, 0), true);
			RenderTextEditor();
		ImGui::EndChild();
	
		ImGui::SameLine();
		ImGui::InvisibleButton("vsplitter", ImVec2(5.0f, ImGui::GetWindowContentRegionMax().y));
		const bool bIsActive = ImGui::IsItemActive();
		const bool bIsHovered = ImGui::IsItemHovered();
		if (bIsActive)
			config->textEditorWidth += ImGui::GetIO().MouseDelta.x;
		if (bIsActive || bIsHovered)
			ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeEW);
		ImGui::SameLine();

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2());
		ImGui::BeginChild("ViewportChild", ImGui::GetContentRegionAvail(), true);
			_viewport.RenderByImGui(ImGui::GetWindowSize());
			if (ImGui::IsItemHovered())
			{
				if (ImGui::IsMouseDown(ImGuiMouseButton_Right))
				{
					ImVec2 mouseDelta = ImGui::GetMouseDragDelta(ImGuiMouseButton_Right);
					_viewport.MouseMoved(mouseDelta);
					if (mouseDelta.x != 0 || mouseDelta.y != 0)
						ImGui::ResetMouseDragDelta(ImGuiMouseButton_Right);
				}
				if (ImGui::GetIO().MouseWheel != 0)
					_viewport.Zoom(ImGui::GetIO().MouseWheel * (ImGui::IsKeyDown(ImGuiKey_LeftShift) ? 5.f : .5f));
			}
		ImGui::EndChild();
		ImGui::PopStyleVar(1);
	
	ImGui::EndChild();

	bool unused_open;
	if (ImGui::BeginPopupModal("BuildTabParamsSelection", &unused_open, ImGuiWindowFlags_AlwaysAutoResize))
	{
		ImGui::BeginChild("PathContainer", ImVec2(0, 40));
		static BuildTabParams params;
		ImGui::InputText("##resPath1", params.saveFilepath.data(), params.filepathLen, ImGuiInputTextFlags_ReadOnly | ImGuiInputTextFlags_AutoSelectAll);
		ImGui::SameLine();
		if (ImGui::Button("Select"))
		{
			std::string filepath = SaveFileDialog("Binary(.bin)\0*.bin\0\0");
			if (!filepath.empty())
			{
				params.saveFilepath = filepath;
				if (!filepath.ends_with(".bin"))
					params.saveFilepath += ".bin";
			}
			Logger::Log(params.saveFilepath);
		}
		ImGui::EndChild();

		std::string depthLabel = std::to_string(static_cast<int>(pow(2, params.recursions))) + " (voxels per axis)";
		if (ImGui::InputInt(depthLabel.c_str(), &params.recursions, 1, 100, ImGuiInputTextFlags_AlwaysInsertMode))
		{
			if (params.recursions > params.maxDepth)
				params.recursions = params.maxDepth;
			else if(params.recursions < params.minDepth)
				params.recursions = params.minDepth;
		}
        ImGui::Checkbox("Enable batching", &params.batching);

		if (params.batching)
		{
			std::string batchLabel = std::to_string((pow(2, params.batchSize) * sizeof(MImage3D)) / 1024. / 1024. / 1024.) + " GB memory";
			if (ImGui::InputInt(batchLabel.c_str(), &params.batchSize, 1, 100, ImGuiInputTextFlags_AlwaysInsertMode))
			{
				if (params.batchSize < params.batchMin)
					params.batchSize = params.batchMin;
			}
		}

		ImGui::DragFloat3("Space center", params.spaceCenter.data(), 0.1f, 0.001f, 0, "%.2f", ImGuiSliderFlags_AlwaysClamp);
		if (ImGui::DragFloat3("Space size", params.spaceSize.data(), 0.05f, 0.1f, 0, "%.2f", ImGuiSliderFlags_AlwaysClamp))
		{
			if (params.spaceSize[0] < 0.1f) params.spaceSize[0] = 0.1f;
			if (params.spaceSize[1] < 0.1f) params.spaceSize[1] = 0.1f;
			if (params.spaceSize[2] < 0.1f) params.spaceSize[2] = 0.1f;
		}

		if (ImGui::Button("Start") && !params.saveFilepath.empty())
		{
			BuildTab(currentActiveTab, params);
			ImGui::CloseCurrentPopup();
			params.saveFilepath.clear();
		}
		ImGui::EndPopup();
	}
}

void ModelingModule::RenderTextEditor()
{
	if (_textEditorTabs.empty())
		return;

	ImGui::BeginTabBar("##textEditorTabs");
	for (size_t i = 0; i < _textEditorTabs.size(); ++i)
	{
		bool bOpen = true;
		if (_textEditorTabs[i].editor.IsTextChanged())
			OnTextChanged(_textEditorTabs[i]);
		
		ImGuiTabItemFlags flags = ImGuiTabItemFlags_None;
		if (_textEditorTabs[i].editor.CanUndo())
			flags |= ImGuiTabItemFlags_UnsavedDocument;
		
		ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10, 10));
		const bool tabOpened = ImGui::BeginTabItem(_textEditorTabs[i].filename.c_str(), &bOpen, flags);
		ImGui::PopStyleVar();
		if (tabOpened)
		{
			currentActiveTab = static_cast<int>(i);
			ImGui::SetWindowFontScale(config->fontSize / config->renderFontSize);
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
		TextEditor::ErrorMarkers markers;
		for (const Parser::Error& error : parser.Errors())
		{
			markers[error.line] = error.text; 
			Logger::Error(error.text);
		}
		currentTab.editor.SetErrorMarkers(markers);
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

void ModelingModule::BuildTab(int tabId, const BuildTabParams& params)
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
		TextEditor::ErrorMarkers markers;
		for (const Parser::Error& error : parser.Errors())
		{
			markers[error.line] = error.text; 
			Logger::Error(error.text);
		}
		currentTab.editor.SetErrorMarkers(markers);
		return;
	}
	
	if (!tree.Root())
	{
		Logger::Error("No main function founded");
		return;
	}
	
	std::optional<std::string> clCode = _openclGenerator.Generate(tree);
	if (!clCode)
	{
		Logger::Error("Generation failed");
		return;
	}

	if (true)
	{
		using namespace ModelingModule_Local;

		Opencl::Executor exec;
		int res = exec.Compile(clCode.value());
		if (res != CL_SUCCESS)
		{
			Logger::Error(fmt::format("OpenCL compilation error: {}\n{}", res, clCode.value()));
			return;
		}
		MultiDimSpace space(params.spaceCenter, params.spaceSize, params.recursions);
		std::ofstream file(params.saveFilepath, std::ios_base::binary);
		auto calculateCallback = [this, &file](size_t start, size_t count)
		{
			if (!imageBuffer.WritePart(file, count))
				Logger::Error("File writing failed");
		};

		size_t batchSize = 0;
		if (file)
		{
			file << space;

			cl_int startId = 0;
			cl_uint3 spaceSize = {static_cast<unsigned>(space.GetPartition()[0]),
								  static_cast<unsigned>(space.GetPartition()[1]),
								  static_cast<unsigned>(space.GetPartition()[2])};

			cl_double3 startPoint = {space.GetStartPoint()[0], space.GetStartPoint()[1], space.GetStartPoint()[2]};
			cl_double3 pointSize = {space.GetUnitSize()[0], space.GetUnitSize()[1], space.GetUnitSize()[2]};
			cl_double3 halfSize = {pointSize.x / 2., pointSize.y / 2., pointSize.z / 2.};

			size_t spaceFlatSize = space.GetTotalPartition();
			size_t bufferSize;
			if (batchSize != 0 && batchSize < spaceFlatSize)
				bufferSize = batchSize;
			else
				bufferSize = spaceFlatSize;

			imageBuffer.Resize(bufferSize);
			// for (size_t i = 0; i < bufferSize; ++i)
			// imageBuffer[i] = {0, 0, 0, 0, 0};

			std::vector<Opencl::KernelArguments::Argument> opt
			{
		        {&startId, sizeof(cl_int)},
				{&spaceSize, sizeof(cl_uint3)},
				{&startPoint, sizeof(cl_double3)},
				{&pointSize, sizeof(cl_double3)},
				{&halfSize, sizeof(cl_double3)},
			};

			Opencl::KernelArguments::Argument result(&imageBuffer[0], sizeof(imageBuffer[0]), imageBuffer.Size());

			int retCode = CL_SUCCESS;
			for (size_t mStartId = 0; mStartId < spaceFlatSize; mStartId += bufferSize)
			{
				retCode = exec.ExecuteCurrentKernel(OpenclGenerator::sKernelProgram, Opencl::KernelArguments(result, opt));

				if (retCode != CL_SUCCESS)
					break;

				if (mStartId + bufferSize > spaceFlatSize)
					bufferSize = spaceFlatSize - mStartId;

				calculateCallback(mStartId, bufferSize);
			}

			if (retCode == CL_SUCCESS)
				Logger::Log("MImage build succeed");
			else
				Logger::Error(fmt::format("MImage build failed: {}", retCode));

			file.close();
		}
	}
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
	
	if (!info.editor.GetErrorMarkers().empty())
		info.editor.SetErrorMarkers({});
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
	_viewport.UpdateImage();
}
}
