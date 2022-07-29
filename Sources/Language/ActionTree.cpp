#include "ActionTree.h"


ActionTree::ActionTree()
{
	InitGlobalConstants();
}

void ActionTree::InitGlobalConstants()
{
	
}

void ActionTree::SetRoot(ActionNode* node)
{
	if (node)
		_root = node;
}

bool ActionTree::AddFunction(FunctionDeclarationNode* func)
{
	if (!GetFunction(func->Name()))
	{
		_functions[func->Name()] = func;
		return true;
	}
	
	return false;
}

FunctionDeclarationNode* ActionTree::GetFunction(const std::string& name) const
{
	auto it = _functions.find(name);
	if (it != _functions.end())
		return it->second;
	
	return nullptr;
}

void ActionTree::AddVariable(VariableDeclarationNode* var)
{
	if (!GetVariable(var->Name()))
		_variables[var->Name()] = var;
}

VariableDeclarationNode* ActionTree::GetVariable(const std::string& name) const
{
	auto it = _variables.find(name);
	if (it != _variables.end())
		return it->second;
	
	return nullptr;
}
