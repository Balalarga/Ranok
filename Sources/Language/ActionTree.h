#pragma once

#include <memory>

#include "ActionNode.h"


class ActionTree
{
public:
	ActionTree();

	void InitGlobalConstants();
	
	template<class T, class ...TArgs>
	std::enable_if_t<std::derived_from<T, ActionNode>, T*> Add(TArgs&& ...args)
	{
		_nodes.push_back(std::make_unique<T>(args...));
		return static_cast<T*>(_nodes.back().get());
	}
	
	template<class T = ActionNode>
	std::enable_if_t<std::derived_from<T, ActionNode>, T*> Last() const
	{
		return _nodes.empty() ? nullptr : dynamic_cast<T*>(_nodes.back().get());
	}
	
private:
	std::vector<std::unique_ptr<ActionNode>> _nodes;
	
	std::vector<UnaryNode*> _arguments;
	std::vector<UnaryNode*> _variables;
	std::vector<DoubleNumberNode*> _numbers;
	std::vector<IntNumberNode*> _indices;
	std::vector<FunctionNode*> _functions;
};
