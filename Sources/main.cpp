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
	();
	g[3]-2;
	return s;
	)";
	
	Lexer lexer(text);
	Parser parser;
	ActionTree tree = parser.Parse(lexer);
	if (parser.HasErrors())
		for (const string& error : parser.Errors())
			cout << error << endl;

	cout << "\nWalking\n";
	if (!tree.Root())
	{
		cout << "Root is empty\n";
		return 0;
	}
	std::queue<ActionNode*> walkQueue = tree.Root()->WalkDown();
	while (!walkQueue.empty())
	{
		ActionNode* top = walkQueue.front();
		if ( top )
		{
			if (auto casted = dynamic_cast<DoubleNumberNode*>(top))
				cout << " = " << casted->Value();
		}
		else
		{
			cout << "nullptr";
		}
		cout << endl;
		walkQueue.pop();
	}
	
	return 0;
}