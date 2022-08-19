#include <iostream>
#include "Language/Lexer.h"
#include "Language/Parser.h"
#include "Language/Generators/IGenerator.h"
#include "Utils/FileUtils.h"

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

int main(int argc, char** argv)
{
	std::map<std::string, std::function<bool()>> tests
	{
		{ "-langTest", []{ return TestLanguage("NewCodeExample/CodeExample1.txt", {"BaseLib.txt"}); } }
	};


	
	std::vector<std::string> success;
	std::vector<std::string> failure;
	for (int i = 1; i < argc; ++i)
	{
		auto it = tests.find(argv[i]);
		if (it != tests.end())
		{
			if (it->second())
				success.emplace_back(argv[i]);
			else
				failure.emplace_back(argv[i]);
		}
	}
	if (!success.empty())
	{
		cout << "Succeed: ";
		for (auto& test: success)
			cout << '\t' << test << '\n';
	}

	if (!success.empty())
	{
		cout << "Failed: ";
		for (auto& test: failure)
			cout << '\t' << test << '\n';
	}
	if (!success.empty() && !failure.empty())
		cout << "(" << success.size() / (success.size() + failure.size()) << ")";
	else
		cout << "No tests made\n";
	return 0;
}