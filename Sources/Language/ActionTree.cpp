#include "ActionTree.h"


ActionTree::ActionTree()
{
	InitGlobalConstants();
}

void ActionTree::InitGlobalConstants()
{
	
}

void ActionTree::IterateNodes(void(* func)(ActionNode*))
{
	for (std::unique_ptr<ActionNode>& node : _nodes)
	{
		if (node)
			func(node.get());
	}
}

void ActionTree::SetRoot(ActionNode* node)
{
	if (_root)
		_root = node;
}

void ActionTree::AddFunction(FunctionDeclarationNode* func)
{
	std::vector<FunctionDeclarationNode*>::iterator it = std::find(_functions.begin(), _functions.end(), func);
	if (it == _functions.end())
		_functions.push_back(func);
}
