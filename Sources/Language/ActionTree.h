#pragma once
#include <memory>

#include "ActionNode.h"


class ActionTree
{
public:
	template<class T, class ...TArgs>
	T& Add(TArgs&& ...args)
	{
		return *_nodes.emplace_back(std::forward<TArgs>(args...)).get();
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
