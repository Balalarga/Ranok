#include "ActionTree.h"

namespace Ranok
{
ActionTree::ActionTree()
{
}

void ActionTree::SetRoot(ActionNode* node)
{
	if (node)
		_root = node;
}

bool ActionTree::AddGlobalFunction(FunctionDeclarationNode* func)
{
	if (!GetGlobalFunction(func->Name()))
	{
		_globalFunctions[func->Name()] = func;
		return true;
	}
	
	return false;
}

FunctionDeclarationNode* ActionTree::GetGlobalFunction(const std::string& name) const
{
	auto it = _globalFunctions.find(name);
	if (it != _globalFunctions.end())
		return it->second;
	
	return nullptr;
}

void ActionTree::AddGlobalVariable(VariableDeclarationNode* var)
{
	if (!GetGlobalVariable(var->Name()))
		_globalVariables[var->Name()] = var;
}

VariableDeclarationNode* ActionTree::GetGlobalVariable(const std::string& name) const
{
	auto it = _globalVariables.find(name);
	if (it != _globalVariables.end())
		return it->second;
	
	return nullptr;
}
}