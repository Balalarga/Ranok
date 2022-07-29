#include <iostream>
#include "Language/Lexer.h"
#include "Language/Parser.h"
using namespace std;


void PrintNode(const ActionNode* node, int depthDelimStep = 2, const std::string& depthDelim = "_", int depth = 0)
{
	if ( node )
	{
		for (int i = 0; i < depth; ++i)
			cout << depthDelim;
		cout << node->Name();
		if (auto casted = dynamic_cast<const DoubleNumberNode*>(node))
			cout << " = " << casted->Value();
		std::queue<ActionNode*> children = node->WalkDown();
		while (!children.empty())
		{
			cout << endl;
			PrintNode(children.front(), depthDelimStep, depthDelim, depth + depthDelimStep);
			children.pop();
		}
	}
	else
	{
		cout << "nullptr";
	}
	cout << endl;
}

int main(int argc, char** argv)
{
	std::string text = R"(
	// asgqwegqaweg
	// agawgaesg
	def s2(s3) { return s3 * 5; }
    var s = (2 + 3) * 4;
	return s +  5;
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

	PrintNode(tree.Root());
	
	return 0;
}