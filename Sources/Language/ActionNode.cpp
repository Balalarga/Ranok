#include "ActionNode.h"

#include <string>

ActionNode::ActionNode(Token token):
	_token(std::move(token))
{
}

std::queue<ActionNode*> ActionNode::WalkDown()
{
	return {};
}

DoubleNumberNode::DoubleNumberNode(const Token& token):
	ActionNode(token),
	_value(std::stod(token.string))
{
}

IntNumberNode::IntNumberNode(const Token& token):
	ActionNode(token),
	_value(std::stoi(token.string))
{
}

UnaryNode::UnaryNode(const Token& token, ActionNode* child):
	ActionNode(token),
	_child(child)
{
}

std::queue<ActionNode*> UnaryNode::WalkDown()
{
	return std::queue<ActionNode*>({ _child });
}

BinaryNode::BinaryNode(const Token& token, ActionNode* left, ActionNode* right):
	ActionNode(token),
	_left(left),
	_right(right)
{
}

std::queue<ActionNode*> BinaryNode::WalkDown()
{
	return std::queue<ActionNode*>({ _left, _right });
}

FunctionNode::FunctionNode(const Token& token, std::vector<ActionNode*> arguments, ActionNode* result):
	ActionNode(token),
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
