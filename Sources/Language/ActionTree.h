#pragma once

#include "ActionNode.h"


namespace Ranok
{
class ActionTree
{
public:
	ActionTree();
	
	const ActionNode* Root() { return _root; }
	void SetRoot(ActionNode* node);
	
	bool AddGlobalFunction(FunctionDeclarationNode* func);
	FunctionDeclarationNode* GetGlobalFunction(const std::string& name) const;
	
	void AddGlobalVariable(VariableDeclarationNode* var);
	VariableDeclarationNode* GetGlobalVariable(const std::string& name) const;
	
	ActionNodeFactory& GlobalFactory() { return _globalFactory; }
	
	
private:
	ActionNodeFactory _globalFactory;
	ActionNode* _root{};
	
	std::map<std::string, FunctionDeclarationNode*> _globalFunctions;
	std::map<std::string, VariableDeclarationNode*> _globalVariables;
};
}