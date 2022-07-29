#pragma once

#include <map>
#include <memory>

#include "ActionNode.h"


class ActionTree
{
public:
	ActionTree();
	
	void InitGlobalConstants();
	
	template<class T, class ...TArgs>
	std::enable_if_t<std::derived_from<T, ActionNode>, T*> Create(TArgs&& ...args)
	{
		_nodes.push_back(std::make_unique<T>(args...));
		return static_cast<T*>(_nodes.back().get());
	}
	
	template<class T>
	void IterateNodes(T&& func)
	{
		for (std::unique_ptr<ActionNode>& node : _nodes)
		{
			if (node)
				func(node.get());
		}
	}
	
	template<class T>
	ActionNode* FindBy(T&& func)
	{
		for (std::unique_ptr<ActionNode>& node : _nodes)
		{
			if (node && func(node.get()))
				return node.get();
		}
		return nullptr;
	}
	
	const ActionNode* Root() { return _root; }
	void SetRoot(ActionNode* node);
	
	bool AddFunction(FunctionDeclarationNode* func);
	FunctionDeclarationNode* GetFunction(const std::string& name) const;
	
	void AddVariable(VariableDeclarationNode* var);
	VariableDeclarationNode* GetVariable(const std::string& name) const;
	
private:
	ActionNode* _root{};
	
	std::vector<std::unique_ptr<ActionNode>> _nodes;
	std::map<std::string, FunctionDeclarationNode*> _functions;
	std::map<std::string, VariableDeclarationNode*> _variables;
};
