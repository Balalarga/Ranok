#ifndef __MINGW__
#define SDL_MAIN_HANDLED
#endif

#include <iostream>

#include "imgui.h"
#include "ImGuiFileDialog.h"

#include "Editor/Editor.h"
#include "Editor/Modules/EditorModule.h"
#include "Editor/Modules/Logger/LoggerModule.h"
#include "Editor/Modules/MImageComputeModule/MImageComputeModule.h"
#include "Editor/Modules/ModelingModule/ModelingModule.h"

#include "Log/Logger.h"
#include "Language/Lexer.h"
#include "Language/Parser.h"
#include "Language/Generators/IGenerator.h"
#include "Utils/FileUtils.h"
#include "Executor/OpenclExecutor.h"

#include "Language/Generators/OpenclGenerator.h"
#include "Localization/LocalizationSystem.h"

#include "Config/IConfig.h"
#include "Config/ConfigManager.h"

#include "Utils/TextureManager.h"
#include "Utils/Archives/Json/JsonArchive.h"

using namespace std;
using namespace Ranok;


struct ScopedCout
{
    ScopedCout(const std::string& inBeginning, std::string&& inEnding): _inEnding(std::move(inEnding))
    {
        if (!inBeginning.empty())
            cout << inBeginning;
    }
    ~ScopedCout()
    {
        if (!_inEnding.empty())
            cout << _inEnding;
    }
private:
    std::string _inEnding;
};

template<class T>
struct ScopedExec
{
    ScopedExec(const T& func): _func(func) {}
    ~ScopedExec() { _func(); }

private:
    const T& _func;
};

std::optional<ActionTree> ReadCode(const std::string& codeAssetPath, const std::vector<std::string>& libAssetPaths)
{
	std::optional<std::string> code = Files::ReadAsset(codeAssetPath);
	if (!code)
	{
		cerr << "File at " << codeAssetPath << " not found\n";
		return {};
	}
	
	Lexer lexer(code.value());
	Parser parser;
	for (const std::string& libPath : libAssetPaths)
	{
		std::optional<ActionNodeFactory> lib = Files::LoadAssetLibrary(libPath);
		if (!lib.has_value())
		{
			cerr << "Couldn't load " << libPath << " library\n";
			return {};
		}
		parser.AddGlobalData(lib.value());
	}
	ActionTree tree = parser.Parse(lexer);
	if (parser.HasErrors())
	{
		for (const string& error : parser.Errors())
			cerr << error << endl;
	}
	
	if (!tree.Root())
	{
		cerr << "No main function founded\n";
		return {};
	}
	
	return tree;
}

bool TestLanguage(const std::string& codeAssetPath, const std::vector<std::string>& libAssetPaths = {})
{
	ScopedCout _("--------LangTest Start-------\n", "--------LangTest End-------\n");
	std::optional<ActionTree> tree = ReadCode(codeAssetPath, libAssetPaths);
	if (!tree.has_value())
		return false;
	
	CppGenerator gen;
	if (std::optional<std::string> res = gen.Generate(tree.value()))
	{
		cout << "Result:\n" << res.value() << endl;
		return true;
	}
	
	cerr << "Generation failed\n";
	return false;
}

void CustomStyle()
{
	auto &style = ImGui::GetStyle();
	style.WindowPadding                     = ImVec2(8.00f, 8.00f);
	style.FramePadding                      = ImVec2(5.00f, 2.00f);
	style.CellPadding                       = ImVec2(6.00f, 6.00f);
	style.ItemSpacing                       = ImVec2(6.00f, 6.00f);
	style.ItemInnerSpacing                  = ImVec2(6.00f, 6.00f);
	style.TouchExtraPadding                 = ImVec2(0.00f, 0.00f);
	style.IndentSpacing                     = 25;
	style.ScrollbarSize                     = 15;
	style.GrabMinSize                       = 10;
	style.WindowBorderSize                  = 1;
	style.ChildBorderSize                   = 1;
	style.PopupBorderSize                   = 1;
	style.FrameBorderSize                   = 1;
	style.TabBorderSize                     = 1;
	style.WindowRounding                    = 7;
	style.ChildRounding                     = 4;
	style.FrameRounding                     = 3;
	style.PopupRounding                     = 4;
	style.ScrollbarRounding                 = 9;
	style.GrabRounding                      = 3;
	style.LogSliderDeadzone                 = 4;
	style.TabRounding                       = 4;
	
    ImVec4 *colors = ImGui::GetStyle().Colors;
	colors[ImGuiCol_Text]                   = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
	colors[ImGuiCol_TextDisabled]           = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
	colors[ImGuiCol_WindowBg]               = ImVec4(0.10f, 0.10f, 0.10f, 1.00f);
	colors[ImGuiCol_ChildBg]                = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_PopupBg]                = ImVec4(0.19f, 0.19f, 0.19f, 0.92f);
	colors[ImGuiCol_Border]                 = ImVec4(0.19f, 0.19f, 0.19f, 0.29f);
	colors[ImGuiCol_BorderShadow]           = ImVec4(0.00f, 0.00f, 0.00f, 0.24f);
	colors[ImGuiCol_FrameBg]                = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
	colors[ImGuiCol_FrameBgHovered]         = ImVec4(0.19f, 0.19f, 0.19f, 0.54f);
	colors[ImGuiCol_FrameBgActive]          = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
	colors[ImGuiCol_TitleBg]                = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
 	colors[ImGuiCol_TitleBgActive]          = ImVec4(0.06f, 0.06f, 0.06f, 1.00f);
	colors[ImGuiCol_TitleBgCollapsed]       = ImVec4(0.00f, 0.00f, 0.00f, 1.00f);
	colors[ImGuiCol_MenuBarBg]              = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
	colors[ImGuiCol_ScrollbarBg]            = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
	colors[ImGuiCol_ScrollbarGrab]          = ImVec4(0.34f, 0.34f, 0.34f, 0.54f);
	colors[ImGuiCol_ScrollbarGrabHovered]   = ImVec4(0.40f, 0.40f, 0.40f, 0.54f);
	colors[ImGuiCol_ScrollbarGrabActive]    = ImVec4(0.56f, 0.56f, 0.56f, 0.54f);
	colors[ImGuiCol_CheckMark]              = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
	colors[ImGuiCol_SliderGrab]             = ImVec4(0.34f, 0.34f, 0.34f, 0.54f);
	colors[ImGuiCol_SliderGrabActive]       = ImVec4(0.56f, 0.56f, 0.56f, 0.54f);
	colors[ImGuiCol_Button]                 = ImVec4(0.05f, 0.05f, 0.05f, 0.54f);
	colors[ImGuiCol_ButtonHovered]          = ImVec4(0.19f, 0.19f, 0.19f, 0.54f);
	colors[ImGuiCol_ButtonActive]           = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
	colors[ImGuiCol_Header]                 = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
	colors[ImGuiCol_HeaderHovered]          = ImVec4(0.00f, 0.00f, 0.00f, 0.36f);
	colors[ImGuiCol_HeaderActive]           = ImVec4(0.20f, 0.22f, 0.23f, 0.33f);
	colors[ImGuiCol_Separator]              = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
	colors[ImGuiCol_SeparatorHovered]       = ImVec4(0.44f, 0.44f, 0.44f, 0.29f);
	colors[ImGuiCol_SeparatorActive]        = ImVec4(0.40f, 0.44f, 0.47f, 1.00f);
	colors[ImGuiCol_ResizeGrip]             = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
	colors[ImGuiCol_ResizeGripHovered]      = ImVec4(0.44f, 0.44f, 0.44f, 0.29f);
	colors[ImGuiCol_ResizeGripActive]       = ImVec4(0.40f, 0.44f, 0.47f, 1.00f);
	colors[ImGuiCol_Tab]                    = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
	colors[ImGuiCol_TabHovered]             = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
	colors[ImGuiCol_TabActive]              = ImVec4(0.20f, 0.20f, 0.20f, 0.36f);
	colors[ImGuiCol_TabUnfocused]           = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
	colors[ImGuiCol_TabUnfocusedActive]     = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
	colors[ImGuiCol_DockingPreview]         = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
	colors[ImGuiCol_DockingEmptyBg]         = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
	colors[ImGuiCol_PlotLines]              = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
	colors[ImGuiCol_PlotLinesHovered]       = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
	colors[ImGuiCol_PlotHistogram]          = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
	colors[ImGuiCol_PlotHistogramHovered]   = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
	colors[ImGuiCol_TableHeaderBg]          = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
	colors[ImGuiCol_TableBorderStrong]      = ImVec4(0.00f, 0.00f, 0.00f, 0.52f);
	colors[ImGuiCol_TableBorderLight]       = ImVec4(0.28f, 0.28f, 0.28f, 0.29f);
	colors[ImGuiCol_TableRowBg]             = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_TableRowBgAlt]          = ImVec4(1.00f, 1.00f, 1.00f, 0.06f);
	colors[ImGuiCol_TextSelectedBg]         = ImVec4(0.20f, 0.22f, 0.23f, 1.00f);
	colors[ImGuiCol_DragDropTarget]         = ImVec4(0.33f, 0.67f, 0.86f, 1.00f);
	colors[ImGuiCol_NavHighlight]           = ImVec4(1.00f, 0.00f, 0.00f, 1.00f);
	colors[ImGuiCol_NavWindowingHighlight]  = ImVec4(1.00f, 0.00f, 0.00f, 0.70f);
	colors[ImGuiCol_NavWindowingDimBg]      = ImVec4(1.00f, 0.00f, 0.00f, 0.20f);
	colors[ImGuiCol_ModalWindowDimBg]       = ImVec4(0.00f, 0.00f, 0.00f, 0.2f);
	
	std::string ICON_IGFD_ADD = "\uf067";
	std::string ICON_IGFD_BOOKMARK = "\uf02e";
	std::string ICON_IGFD_CANCEL = "\uf00d";
	std::string ICON_IGFD_CHEVRON_DOWN = "\uf078";
	std::string ICON_IGFD_CHEVRON_UP = "\uf077";
	std::string ICON_IGFD_DRIVES = "\uf0a0";
	std::string ICON_IGFD_EDIT = "\uf040";
	std::string ICON_IGFD_FILE = "\uf15b";
	std::string ICON_IGFD_FILE_GRID_THUMBNAILS = "\uf00a";
	std::string ICON_IGFD_FILE_LIST = "\uf0c9";
	std::string ICON_IGFD_FILE_LIST_THUMBNAILS = "\uf00b";
	std::string ICON_IGFD_FILE_PIC = "\uf1c5";
	std::string ICON_IGFD_FOLDER = "\uf07b";
	std::string ICON_IGFD_FOLDER_OPEN = "\uf07c";
	std::string ICON_IGFD_LINK = "\uf1c9";
	std::string ICON_IGFD_OK = "\uf00c";
	std::string ICON_IGFD_REFRESH = "\uf021";
	std::string ICON_IGFD_REMOVE = "\uf068";
	std::string ICON_IGFD_RESET = "\uf064";
	std::string ICON_IGFD_SAVE = "\uf0c7";
	std::string ICON_IGFD_SEARCH = "\uf002";
	std::string ICON_IGFD_STEP_FORWARD = "\uf051";
	
	ImGuiFileDialog::Instance()->SetFileStyle(IGFD_FileStyleByFullName, "(Custom.+[.]h)", ImVec4(1.0f, 1.0f, 0.0f, 0.9f));
	ImGuiFileDialog::Instance()->SetFileStyle(IGFD_FileStyleByExtention, ".cpp", ImVec4(1.0f, 1.0f, 0.0f, 0.9f));
	ImGuiFileDialog::Instance()->SetFileStyle(IGFD_FileStyleByExtention, ".h", ImVec4(0.0f, 1.0f, 0.0f, 0.9f));
	ImGuiFileDialog::Instance()->SetFileStyle(IGFD_FileStyleByExtention, ".hpp", ImVec4(0.0f, 0.0f, 1.0f, 0.9f));
	ImGuiFileDialog::Instance()->SetFileStyle(IGFD_FileStyleByExtention, ".md", ImVec4(1.0f, 0.0f, 1.0f, 0.9f));
	ImGuiFileDialog::Instance()->SetFileStyle(IGFD_FileStyleByExtention, ".png", ImVec4(0.0f, 1.0f, 1.0f, 0.9f), ICON_IGFD_FILE_PIC); // add an icon for the filter type
	ImGuiFileDialog::Instance()->SetFileStyle(IGFD_FileStyleByExtention, ".gif", ImVec4(0.0f, 1.0f, 0.5f, 0.9f), "[GIF]"); // add an text for a filter type
	ImGuiFileDialog::Instance()->SetFileStyle(IGFD_FileStyleByTypeDir, nullptr, ImVec4(0.5f, 1.0f, 0.9f, 0.9f), ICON_IGFD_FOLDER); // for all dirs
	ImGuiFileDialog::Instance()->SetFileStyle(IGFD_FileStyleByTypeFile, "CMakeLists.txt", ImVec4(0.1f, 0.5f, 0.5f, 0.9f), ICON_IGFD_ADD);
	ImGuiFileDialog::Instance()->SetFileStyle(IGFD_FileStyleByFullName, "doc", ImVec4(0.9f, 0.2f, 0.0f, 0.9f), ICON_IGFD_FILE_PIC);
	ImGuiFileDialog::Instance()->SetFileStyle(IGFD_FileStyleByTypeDir | IGFD_FileStyleByContainedInFullName, ".git", ImVec4(0.9f, 0.2f, 0.0f, 0.9f), ICON_IGFD_BOOKMARK);
	ImGuiFileDialog::Instance()->SetFileStyle(IGFD_FileStyleByTypeFile | IGFD_FileStyleByContainedInFullName, ".git", ImVec4(0.5f, 0.8f, 0.5f, 0.9f), ICON_IGFD_SAVE);

	ImGuiIO& io = ImGui::GetIO();
	std::string fontPath = Files::GetAssetPath("Fonts/Roboto-Regular.ttf");
	ImFont* font1 = io.Fonts->AddFontFromFileTTF(fontPath.c_str(), 14, nullptr, io.Fonts->GetGlyphRangesCyrillic());
	io.FontDefault = font1;
}

bool TestGui()
{
	Editor& editor = Editor::Instance();
	editor.Show();
	return true;
}

bool TestOpencl(const std::string& codeAssetPath, const std::vector<std::string>& libAssetPaths = {})
{
	Opencl::Executor::Init();
	ScopedExec openclDestroyer([]{ Opencl::Executor::Destroy(); });
	ScopedCout _("--------OpenCL Start-------\n", "--------OpenCL End-------\n");
	std::optional<ActionTree> tree = ReadCode(codeAssetPath, libAssetPaths);
	if (!tree.has_value())
		return false;
	
	OpenclGenerator gen;
	std::optional<std::string> res = gen.Generate(tree.value());
	if (!res)
	{
		cerr << "Generation failed\n";
		return false;
	}
	
	Opencl::Executor exec;
	int result = exec.Compile(res.value());
	if (result != CL_SUCCESS)
	{
		cerr << result;
		return false;
	}
	
	cout << "Success\n";
	return true;
}

void Preinit()
{
}

void LoadAssets()
{
	auto& textureManager = TextureManager::Instance();
	
	auto TryToLoadTexture = [&textureManager](std::string&& tag, std::string&& assetPath)
	{
		std::string fullPath = Files::GetAssetPath(assetPath);
		std::optional<TextureManager::TextureInfo> textureInfo = textureManager.LoadTexture(tag, fullPath);
		if (!textureInfo.has_value())
			Logger::Warning(fmt::format("Couldn't load texture \"{}\" from {} ", tag, fullPath));
		else
			Logger::Log(fmt::format("Loaded texture \"{}\" from {} ", tag, fullPath));
	};
	TryToLoadTexture("FileIcon", "Icons/FileIcon.png");
	TryToLoadTexture("DirIcon", "Icons/DirIcon.png");
}

void Init()
{
	Editor::Instance();
	LoadAssets();
	CustomStyle();
	
	auto& logger = Editor::EditorSystem.AddModule<LoggerModule>("Log");
	logger.bWorks = true;

	auto& mimageCompute = Editor::EditorSystem.AddModule<MImageComputeModule>();
	
	auto& modeling = Editor::EditorSystem.AddModule<ModelingModule>();
	modeling.bWorks = true;
	
	Editor::EditorSystem.Init();
	Logger::Log("Editor module system inited");
	
	
	class Someconfigs: public IConfig
	{
	public:
		Someconfigs(): IConfig("Someconfigs")
		{
		}
		
		void Serialize(JsonArchive& archive) override
		{
			archive.Serialize("someName", val);
			archive.Serialize("otherName", vec);
		}
		
		std::vector<int> vec;
		int val{};
		float val2{};
	};
	std::shared_ptr<Someconfigs> config = ConfigManager::Instance().Createconfigs<Someconfigs>();
}

void PreDestroy()
{
	ConfigManager::Instance().SaveAll();
}

void Destroy()
{
}

int main(int argc, char** argv)
{
    Preinit();
    Init();
	constexpr const char* mainTest = "-main";
	std::map<std::string, std::function<bool()>> tests
	{
		{ mainTest, TestGui },
		{ "-langTest", []{ return TestLanguage("NewCodeExample/CodeExample1.txt", {"BaseLib.txt"}); } },
		{ "-openclTest", []{ return TestOpencl("NewCodeExample/CodeExample1.txt", {"BaseLib.txt"}); } },
    };
	std::vector<std::string> success;
	std::vector<std::string> failure;
	
    auto tester = [&failure, &success, &mainTest](std::function<bool()>& func)
	{
		if (func())
			success.emplace_back(mainTest);
		else
			failure.emplace_back(mainTest);
	};
	
    if (argc <= 1 && tests.contains(mainTest))
    {
        tester(tests[mainTest]);
    }
    else
    {
        for (int i = 1; i < argc; ++i)
        {
            auto it = tests.find(argv[i]);
            if (it != tests.end())
                tester(it->second);
        }
    }

	if (!success.empty())
	{
		cout << "Succeed:\n";
		for (std::string& result : success)
			cout << "    " << result << '\n';
		cout << "\n";
	}
	
	if (!success.empty())
	{
		cerr << "Failed:\n";
		for (std::string& result : failure)
			cerr << "    " << result << '\n';
		cerr << '\n';
	}
	unsigned long long total = success.size() + failure.size();
	if (total > 0)
	{
		cout << "Passed " << success.size() << "/" << total;
	}
	else
	{
		cout << "No tests made\n";
	}

    PreDestroy();
    Destroy();
	return 0;
}
