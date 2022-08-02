#include <iostream>
#include "Language/Lexer.h"
#include "Language/Parser.h"
#include "Utils/FileUtils.h"
using namespace std;
using namespace Ranok;

void PrintNode(const ActionNode* node, int depthDelimStep = 2, const std::string& depthDelim = " ", int depth = 0)
{
	auto coutPrefix = [&]
	{
		for (int i = 0; i < depth; ++i)
		{
			if (i % depthDelimStep == 0)
				cout << "|";
			else
				cout << depthDelim;
		}
	};
	if (node)
	{
		coutPrefix();
		cout << node->Name();
		if (auto casted = dynamic_cast<const DoubleNumberNode*>(node))
		{
			cout << " = " << casted->Value();
		}
		else if (auto casted = dynamic_cast<const VariableNode*>(node))
		{
			cout << " (var)";
		}
		else if (auto casted = dynamic_cast<const VariableDeclarationNode*>(node))
		{
			cout << " = varDecl: " << casted->Name() << "";
		}
		else if (auto casted = dynamic_cast<const FunctionDeclarationNode*>(node))
		{
			cout << " = funcDecl: " << casted->Name() << "(";
			for (size_t i = 0; i < casted->Signature().Args().size(); ++i)
			{
				cout << casted->Signature().Args()[i]->Name();
				if (i+1 < casted->Signature().Args().size())
					cout << ", ";
			}
			cout << ")";
		}
		else if (auto casted = dynamic_cast<const FunctionCallNode*>(node))
		{
			cout << "{" << casted->Root()->Name() << "(";
			for (size_t i = 0; i < casted->Root()->Signature().Args().size(); ++i)
			{
				cout << casted->Root()->Signature().Args()[i]->Name();
				if (i+1 < casted->Root()->Signature().Args().size())
					cout << ", ";
			}
			cout << ")}\n";
			coutPrefix();
			cout << "(";
			std::queue<ActionNode*> children = node->WalkDown();
			while (!children.empty())
			{
				cout << endl;
				PrintNode(children.front(), depthDelimStep, depthDelim, depth + depthDelimStep);
				children.pop();
			}
			coutPrefix();
			cout << ")";
			return;
		}
		
		std::queue<ActionNode*> children = node->WalkDown();
		while (!children.empty())
		{
			if (children.front())
			{
				cout << endl;
				PrintNode(children.front(), depthDelimStep, depthDelim, depth + depthDelimStep);
			}
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
	std::string filepath = RESOURCE_DIR"/NewCodeExample/CodeExample1.txt";
	auto code = Ranok::Files::ReadFile(filepath);
	if (!code)
	{
		cout << RESOURCE_DIR << endl;
		cout << "File at " << filepath << " not found\n";
		return -1;
	}
	
	std::string text = R"(
	// asgqwegqaweg
	def s(se)
	{
		return se;
	}
	def main(space[3])
	{
		x = 5;
		def sw(e) { return e + x }
	    var s3 = s(2 + 3) * sw(4+5)
		return s3 + x;
	})";
	
	Lexer lexer(text);
	Parser parser;
	ActionTree tree = parser.Parse(lexer);
	if (parser.HasErrors())
		for (const string& error : parser.Errors())
			cout << error << endl;
	
	if (!tree.Root())
	{
		cout << "Root is empty\n";
		return 0;
	}
	cout << "Program:\n";
	PrintNode(tree.Root());
	
	return 0;
}