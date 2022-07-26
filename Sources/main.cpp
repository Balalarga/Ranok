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

	cout << "\nWalking\n";
	std::queue<ActionNode*> walkQueue = tree.Last()->WalkDown();
	while (!walkQueue.empty())
	{
		ActionNode* top = walkQueue.front();
		
		cout << top->GetToken().string;
		if (auto casted = dynamic_cast<DoubleNumberNode*>(top))
			cout << " = " << casted->Value();
		cout << endl;
		walkQueue.pop();
	}
	
	return 0;
}