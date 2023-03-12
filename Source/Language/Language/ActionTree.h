#pragma once

#include "ActionNode.h"


namespace Ranok
{
class ActionTree
{
public:
	ActionTree() = default;
	
	const ActionNode* Root() const { return _root; }
	void SetRoot(ActionNode* node) { if (node) _root = node; }
	
	ActionNodeFactory& GlobalFactory() { return _globalFactory; }
	const ActionNodeFactory& GlobalFactory() const { return _globalFactory; }
	
	
private:
	ActionNodeFactory _globalFactory;
	ActionNode* _root{};
};
}