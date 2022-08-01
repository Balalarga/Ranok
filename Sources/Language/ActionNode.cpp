#include "ActionNode.h"
#include <utility>

namespace Ranok
{
ActionNodeFactory ActionNodeFactory::operator+(const ActionNodeFactory& oth)
{
	ActionNodeFactory factory;
	for (auto& node: _nodes)
		factory._nodes.push_back();
	
	return *this;
}

VariableDeclarationNode* ActionNodeFactory::CreateVariable(const std::string& name, ActionNode* value)
{
	if (VariableDeclarationNode* var = FindVariable(name))
		return var;
	
	return _variables.insert({name, Create<VariableDeclarationNode>(name, value)}).first->second;	
}

FunctionDeclarationNode* ActionNodeFactory::CreateFunction(const FunctionSignature& signature, ActionNode* body)
{
	if (FunctionDeclarationNode* func = FindFunction(signature.Name()))
		return func;
	
	return _functions.insert({signature.Name(), Create<FunctionDeclarationNode>(signature, body)}).first->second;
}

VariableDeclarationNode* ActionNodeFactory::FindVariable(const std::string& name) const
{
	auto it = _variables.find(name);
	if (it == _variables.end())
		return nullptr;
	
	return it->second;
}
FunctionDeclarationNode* ActionNodeFactory::FindFunction(const std::string& name) const
{
	auto it = _functions.find(name);
	if (it == _functions.end())
		return nullptr;
	
	return it->second;
}

ActionNode::ActionNode(const std::string& name):
	_name(name)
{
	
}

std::queue<ActionNode*> ActionNode::WalkDown() const
{
	return {};
}

DoubleNumberNode::DoubleNumberNode(double number):
	ActionNode(std::to_string(number)),
	_value(number)
{
}

IntNumberNode::IntNumberNode(int number):
	ActionNode(std::to_string(number)),
	_value(number)
{
}

ArrayGetterNode::ArrayGetterNode(const std::string& name, ActionNode* id):
	ActionNode(name),
	_id(id)
{
}

ArrayDeclarationNode::ArrayDeclarationNode(const std::string& name, std::vector<ActionNode*> values):
	ActionNode(name),
	_values(std::move(values))
{
}

std::queue<ActionNode*> ArrayDeclarationNode::WalkDown() const
{
	std::queue<ActionNode*> queue;
	for (ActionNode* val : _values)
		queue.push(val);
	return queue;
}

VariableNode::VariableNode(const std::string& name, VariableDeclarationNode* decl):
	ActionNode(name),
	_declaration(decl)
{
}

std::queue<ActionNode*> VariableNode::WalkDown() const
{
	return _declaration->WalkDown();
}

VariableDeclarationNode::VariableDeclarationNode(const std::string& name, ActionNode* value):
	ActionNode(name),
	_value(value)
{
}

std::queue<ActionNode*> VariableDeclarationNode::WalkDown() const
{
	return std::queue<ActionNode*>{ { _value } };
}

UnaryNode::UnaryNode(const std::string& name, ActionNode* child):
	ActionNode(name),
	_child(child)
{
}

std::queue<ActionNode*> UnaryNode::WalkDown() const
{
	return std::queue<ActionNode*>({ _child });
}

BinaryNode::BinaryNode(const std::string& name, ActionNode* left, ActionNode* right):
	ActionNode(name),
	_left(left),
	_right(right)
{
}

std::queue<ActionNode*> BinaryNode::WalkDown() const
{
	return std::queue<ActionNode*>({ _left, _right });
}

FunctionCallNode::FunctionCallNode(const std::string& name, std::vector<ActionNode*> arguments):
	ActionNode(name),
	_arguments(std::move(arguments))
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
	ActionNode(signature.Name()),
	_signature(signature),
	_body(body)
{
}
}