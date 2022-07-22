#include "ActionNode.h"

#include <any>
#include <utility>

ActionNode::ActionNode(Token token):
	_token(std::move(token))
{
}

std::queue<ActionNode*> ActionNode::WalkDown()
{
	return {};
}

DoubleNumberNode::DoubleNumberNode(Token token, double value):
	ActionNode(std::move(token)),
	_value(value)
{
}

IntNumberNode::IntNumberNode(Token token, int value):
	ActionNode(std::move(token)),
	_value(value)
{
}

UnaryNode::UnaryNode(Token token, ActionNode* child):
	ActionNode(std::move(token)),
	_child(child)
{
}

std::queue<ActionNode*> UnaryNode::WalkDown()
{
	return std::queue<ActionNode*>({ _child });
}

BinaryNode::BinaryNode(Token token, ActionNode* left, ActionNode* right):
	ActionNode(std::move(token)),
	_left(left),
	_right(right)
{
}

std::queue<ActionNode*> BinaryNode::WalkDown()
{
	return std::queue<ActionNode*>({ _left, _right });
}

FunctionNode::FunctionNode(Token token, std::vector<ActionNode*> arguments, ActionNode* result):
	ActionNode(std::move(token)),
	_arguments(std::move(arguments)),
	_result(result)
{
}

std::queue<ActionNode*> FunctionNode::WalkDown()
{
	std::queue<ActionNode*> queue;
	for (auto& argument: _arguments)
		queue.push(argument);
	queue.push(_result);
	return queue;
}
