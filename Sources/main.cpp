#include <iostream>
#include "Language/Lexer.h"

using namespace std;


int main(int argc, char** argv)
{
	std::string text = R"(
	// asgqwegqaweg
	// agawgaesg
    var s = 2;
	g[3]-2;
	return s;
	)";
	
	Lexer lexer(text);
	
	while (!lexer.IsEmpty())
	{
		cout << lexer.Peek().TypeToString() << " = " << "\"" << lexer.Peek().string <<"\"";
		cout << " (" << lexer.Peek().line << " : " << lexer.Peek().column << ")" << endl;
		lexer.Pop();
	}
	
	return 0;
}