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
		if (dynamic_cast<const IntNumberNode*>(node))
		{
			cout << " (int)\n";
			return;
		}
		else if (dynamic_cast<const DoubleNumberNode*>(node))
		{
			cout << " (double)\n";
			return;
		}
		else if (auto arr = dynamic_cast<const ArrayNode*>(node))
		{
			cout << " [" << arr->Values().size() << "]\n";
			return;
		}
		else if (auto castedVar = dynamic_cast<const VariableNode*>(node))
		{
			cout << " (var)\n";
		}
		else if (auto castedVarDecl = dynamic_cast<const VariableDeclarationNode*>(node))
		{
			cout << " = varDecl: " << castedVarDecl->Name();
		}
		else if (auto castedFunc = dynamic_cast<const FunctionDeclarationNode*>(node))
		{
			cout << " = funcDecl: " << castedFunc->Name() << "(";
			for (size_t i = 0; i < castedFunc->Signature().Args().size(); ++i)
			{
				cout << castedFunc->Signature().Args()[i]->Name();
				if (ArrayNode* arr = castedFunc->Signature().Args()[i]->As<ArrayNode>())
					cout << "[" << arr->Values().size() << "]";
				if (i+1 < castedFunc->Signature().Args().size())
					cout << ", ";
			}
			cout << ")";
		}
		else if (auto castedFuncCall = dynamic_cast<const FunctionCallNode*>(node))
		{
			cout << endl;
			coutPrefix();
			cout << "(\n";
			std::queue<ActionNode*> children = castedFuncCall->WalkDown();
			while (!children.empty())
			{
				PrintNode(children.front(), depthDelimStep, depthDelim, depth + depthDelimStep);
				children.pop();
			}
			coutPrefix();
			cout << ")";
			return;
		}
		else if (auto castedArrGet = dynamic_cast<const ArrayGetterNode*>(node))
		{
			cout << endl;
			coutPrefix();
			cout << "[\n";
			std::queue<ActionNode*> children = castedArrGet->WalkDown();
			if (!children.empty())
				PrintNode(children.front(), depthDelimStep, depthDelim, depth + depthDelimStep);
			coutPrefix();
			cout << "]";
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
		return;
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
	std::optional<std::string> code = Files::ReadFile(filepath);
	if (!code)
	{
		cout << RESOURCE_DIR << endl;
		cout << "File at " << filepath << " not found\n";
		return -1;
	}
	
	Lexer lexer(code.value());
	Parser parser;
	ActionTree tree = parser.Parse(lexer);
	if (parser.HasErrors())
		for (const string& error : parser.Errors())
			cout << error << endl;

	for (auto& func: tree.GlobalFactory().Functions())
	{
		if (func.first == "main")
			continue;
		PrintNode(func.second);
		cout << "\n\n";
	}
	
	if (!tree.Root())
	{
		cout << "No main function founded\n";
		return 0;
	}
	cout << "Program:\n";
	PrintNode(tree.Root());
	
	return 0;
}