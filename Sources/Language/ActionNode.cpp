#include "ActionNode.h"

namespace Ranok
{
std::shared_ptr<ActionNodeFactory::Commitable<FunctionDeclarationNode>> ActionNodeFactory::TempCreateFunction(
	const FunctionSignature& signature)
{
	return std::make_shared<Commitable<FunctionDeclarationNode>>(
		signature.Name(),
		new FunctionDeclarationNode(signature, nullptr),
		[this](const std::string& name, FunctionDeclarationNode* func)
		{
			if (_functions.end() != _functions.find(name))
				return true;
			
			_functions.insert({name, func});
			_declarationOrder.push_back(func);
			return false;
		});
}

VariableDeclarationNode* ActionNodeFactory::CreateVariable(const std::string& name, ActionNode* value)
{
	if (VariableDeclarationNode* var = FindVariable(name))
		return var;

	VariableDeclarationNode* var = _variables.insert({name, Create<VariableDeclarationNode>(name, value)}).first->second;
	_declarationOrder.push_back(var);
	return var;
}

FunctionDeclarationNode* ActionNodeFactory::CreateFunction(const FunctionSignature& signature, ActionNode* body)
{
	if (FunctionDeclarationNode* func = FindFunction(signature.Name()))
		return func;

	FunctionDeclarationNode* func = _functions.insert({signature.Name(), Create<FunctionDeclarationNode>(signature, body)}).first->second;
	_declarationOrder.push_back(func);
	return func;
}

ArrayNode* ActionNodeFactory::FindArrayVariable(const std::string& name)
{
	if (auto var = dynamic_cast<ArrayNode*>(FindVariable(name)->Value()))
		return var;
	
	return nullptr;
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

ArrayGetterNode::ArrayGetterNode(VariableDeclarationNode* var, ActionNode* id):
	ActionNode(var->Name()),
	_var(var),
	_id(id)
{
}

std::queue<ActionNode*> ArrayGetterNode::WalkDown() const
{
	return std::queue<ActionNode*>{ { _id } };
}

ArrayNode::ArrayNode(const std::string& name, const std::vector<ActionNode*>& values):
	ActionNode(name),
	_values(values)
{
}

std::queue<ActionNode*> ArrayNode::WalkDown() const
{
	std::queue<ActionNode*> queue;
	for (ActionNode* val : _values)
		queue.push(val);
	return queue;
}

VariableNode::VariableNode(VariableDeclarationNode* decl):
	ActionNode(decl->Name()),
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
	if (dynamic_cast<ArrayNode*>(_value))
	{
		_type = VariableType::Array;
	}
	else if (dynamic_cast<DoubleNumberNode*>(_value))
	{
		_type = VariableType::Double;
	}
	else
	{
		auto queue = _value->WalkDown();
		while(!queue.empty())
		{
			if (dynamic_cast<ArrayNode*>(queue.front()))
			{
				_type = VariableType::Array;
				break;
			}
			if (dynamic_cast<DoubleNumberNode*>(queue.front()))
				_type = VariableType::Double;
			queue.pop();
		}
	}
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

FunctionCallNode::FunctionCallNode(FunctionDeclarationNode* root, std::vector<ActionNode*> arguments):
	ActionNode(root->Name()),
	_root(root),
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

FunctionSignature::FunctionSignature(const std::string& name, const std::vector<VariableDeclarationNode*>& args):
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

std::queue<ActionNode*> FunctionDeclarationNode::WalkDown() const
{
	return std::queue<ActionNode*>{{_body}};
}
}
