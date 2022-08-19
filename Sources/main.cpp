#include <iostream>
#include "Language/Lexer.h"
#include "Language/Parser.h"
#include "Language/Generators/IGenerator.h"
#include "OpenGL/Core/Scene.h"
#include "Utils/FileUtils.h"
#include "WindowSystem/OpenglWindow.h"

using namespace std;
using namespace Ranok;

struct ScopedCout
{
	ScopedCout(const std::string& inBeginning, const std::string& inEnding): inEnding(inEnding)
	{
		cout << inBeginning;
	}
	~ScopedCout()
	{
		cout << inEnding;
	}
	std::string inEnding;
};

bool TestLanguage(const std::string& codeAssetPath, const std::vector<std::string>& libAssetPaths = {})
{
	ScopedCout _("--------LangTest Start-------\n", "--------LangTest End-------\n");
	std::optional<std::string> code = Files::ReadAsset(codeAssetPath);
	if (!code)
	{
		cerr << "File at " << codeAssetPath << " not found\n";
		return false;
	}
	
	Lexer lexer(code.value());
	Parser parser;
	for (auto& libPath: libAssetPaths)
	{
		std::optional<ActionNodeFactory> lib = Files::LoadAssetLibrary(libPath);
		if (!lib.has_value())
		{
			cerr << "Couldn't load " << libPath << " library\n";
			return false;
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
		return false;
	}
	
	CppGenerator gen;
	if (std::optional<std::string> res = gen.Generate(tree))
	{
		cout << "Result:\n" << res.value() << endl;
		return true;
	}
	
	cerr << "Generation failed\n";
	return false;
}

bool TestGui()
{
	OpenglWindow window({});
	Scene scene;
	window.SetBackgroundColor(glm::vec4(0.2, 0.2, 0.2, 1.0));
	window.SetScene(&scene);
	window.Show();
	return true;
}

int main(int argc, char** argv)
{
	constexpr const char* mainTest = "-main";
	std::map<std::string, std::function<bool()>> tests
	{
		{ mainTest, [](){ return TestGui(); } },
		{ "-langTest", []{ return TestLanguage("NewCodeExample/CodeExample1.txt", {"BaseLib.txt"}); } },
	};


	std::vector<std::string> success;
	std::vector<std::string> failure;
	for (int i = 1; i < argc; ++i)
	{
		auto it = tests.find(argv[i]);
		if (it != tests.end() || strcmp(argv[i], "") == 0)
		{
			if (it->second())
				success.emplace_back(argv[i]);
			else
				failure.emplace_back(argv[i]);
		}
	}
	if (argc == 1 && tests.contains(mainTest))
	{
		if (tests[mainTest]())
			success.emplace_back(mainTest);
		else
			failure.emplace_back(mainTest);
	}
	
	if (!success.empty())
	{
		cout << "Succeed:\n";
		for (auto& test: success)
			cout << "    " << test << '\n';
		cout << "\n";
	}

	if (!success.empty())
	{
		cerr << "Failed:\n";
		for (auto& test: failure)
			cerr << "    " << test << '\n';
		cerr << '\n';
	}
	if (!success.empty() || !failure.empty())
	{
		cout << "Passed " << success.size() / (success.size() + failure.size()) << "/" << (success.size() + failure.size());
	}
	else
	{
		cout << "No tests made\n";
	}
	return 0;
}