#include <iostream>
#include "Language/Lexer.h"
#include "Language/Parser.h"

using namespace std;


int main(int argc, char** argv)
{
	std::string text = R"(
	// asgqwegqaweg
	// agawgaesg
    var s = 2;
	|;
	g[3]-2;
	return s;
	)";
	
	Lexer lexer(text);
	Parser parser;
	ActionTree tree = parser.Parse(lexer);
	ActionNode* last = tree.Last<DoubleNumberNode>();
	if (parser.HasErrors())
		for (const string& error : parser.Errors())
			cout << error << endl;
	
	// while (!lexer.IsEmpty())
	// {
		// cout << lexer.Peek().TypeToString() << " = " << "\"" << lexer.Peek().string <<"\"";
		// cout << " (" << lexer.Peek().line << " : " << lexer.Peek().column << ")" << endl;
		// lexer.Pop();
	// }
	
	return 0;
}