#include "ActionNode.h"


std::queue<ActionNode*> ActionNode::WalkDown() const
{
	return {};
}

DoubleNumberNode::DoubleNumberNode(double number):
	_value(number)
{
}

IntNumberNode::IntNumberNode(int number):
	_value(number)
{
}

ArrayGetterNode::ArrayGetterNode(const std::string& name, ActionNode* id):
	_name(name),
	_id(id)
{
}

ArrayDeclarationNode::ArrayDeclarationNode(const std::string& name, std::vector<ActionNode*> values):
	_name(name),
	_values(values)
{
}

VariableNode::VariableNode(const std::string& name):
	_name(name)
{
}

UnaryNode::UnaryNode(const std::string& name, ActionNode* child):
	_name(name),
	_child(child)
{
}

std::queue<ActionNode*> UnaryNode::WalkDown() const
{
	return std::queue<ActionNode*>({ _child });
}

BinaryNode::BinaryNode(const std::string& name, ActionNode* left, ActionNode* right):
	_name(name),
	_left(left),
	_right(right)
{
}

std::queue<ActionNode*> BinaryNode::WalkDown() const
{
	return std::queue<ActionNode*>({ _left, _right });
}

FunctionCallNode::FunctionCallNode(const std::string& name, std::vector<ActionNode*> arguments):
	_arguments(std::move(arguments)),
	_name(name)
{
}

std::queue<ActionNode*> FunctionCallNode::WalkDown() const
{
	std::queue<ActionNode*> queue;
	for (auto& argument: _arguments)
		queue.push(argument);
	return queue;
}

FunctionSignature::FunctionSignature(const std::string& name, const std::vector<std::string>& args):
	_name(name),
	_arguments(args)
{
}

FunctionDeclarationNode::FunctionDeclarationNode(const FunctionSignature& signature, ActionNode* body):
	_signature(signature),
	_body(body)
{
}

