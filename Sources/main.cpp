#include <iostream>
#include "Language/Lexer.h"
#include "Language/Parser.h"
#include "Language/Generators/IGenerator.h"
#include "Utils/FileUtils.h"

using namespace std;
using namespace Ranok;


int main(int argc, char** argv)
{
	std::string filepath = RESOURCE_DIR"/NewCodeExample/CodeExample1.txt";
	std::optional<std::string> code = Files::ReadFile(filepath);
	if (!code)
	{
		cout << "File at " << filepath << " not found\n";
		return -1;
	}
	
	Lexer lexer(code.value());
	Parser parser;
	ActionTree tree = parser.Parse(lexer);
	if (parser.HasErrors())
	{
		for (const string& error : parser.Errors())
			cout << error << endl;
	}
	
	if (!tree.Root())
	{
		cout << "No main function founded\n";
		return -1;
	}
	
	CppGenerator gen;
	auto res =  gen.Generate(tree);
	if (res.has_value())
		cout << "Result:\n" << res.value() << endl;
    else
        cout << "Generation failed\n";
	
	return 0;
}