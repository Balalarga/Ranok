#include "ActionNode.h"

namespace Ranok
{
std::shared_ptr<ActionNodeFactory::Commitable<FunctionDeclarationNode>> ActionNodeFactory::TempCreateFunction(
	const FunctionSignature& signature)
{
	return std::make_shared<Commitable<FunctionDeclarationNode>>(
		signature.GetToken().string,
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

VariableDeclarationNode* ActionNodeFactory::CreateVariable(const Token& name, ActionNode* value)
{
	if (VariableDeclarationNode* var = FindVariable(name.string))
		return var;

	VariableDeclarationNode* var = _variables.insert({name.string, Create<VariableDeclarationNode>(name, value)}).first->second;
	_declarationOrder.push_back(var);
	return var;
}

FunctionDeclarationNode* ActionNodeFactory::CreateFunction(const FunctionSignature& signature, ActionNode* body)
{
	if (FunctionDeclarationNode* func = FindFunction(signature.GetToken().string))
		return func;

	FunctionDeclarationNode* func = _functions.insert({signature.GetToken().string, Create<FunctionDeclarationNode>(signature, body)}).first->second;
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

ActionNode::ActionNode(const Token& token):
	_token(token)
{
}

const ArrayNode* ActionNode::IsArray(const ActionNode* node)
{
	if (auto arr = dynamic_cast<const ArrayNode*>(node))
		return arr;
	
	if (auto var = dynamic_cast<const VariableNode*>(node))
		return IsArray(var->Declaration()->Value());
	
	if (auto varDecl = dynamic_cast<const VariableDeclarationNode*>(node))
		return IsArray(varDecl->Value());
	
	if (auto funcCall = dynamic_cast<const FunctionCallNode*>(node))
		return IsArray(funcCall->Root()->Body());
	
	return nullptr;
}

size_t ActionNode::GetArraySize(const ActionNode* node)
{
	if (auto arr = dynamic_cast<const ArrayNode*>(node))
		return arr->Values().size();
	
	if (auto var = dynamic_cast<const VariableNode*>(node))
		return GetArraySize(var->Declaration()->Value());
	
	if (auto funcCall = dynamic_cast<const FunctionCallNode*>(node))
		return GetArraySize(funcCall->Root()->Body());
	
	return 0;
}

std::queue<ActionNode*> ActionNode::WalkDown() const
{
	return {};
}

DoubleNumberNode::DoubleNumberNode(const Token& token, double number):
	ActionNode(token),
	_value(number)
{
}

ArrayGetterNode::ArrayGetterNode(VariableDeclarationNode* var, ActionNode* id):
	ActionNode(var->GetToken()),
	_var(var),
	_id(id)
{
}

std::queue<ActionNode*> ArrayGetterNode::WalkDown() const
{
	return std::queue<ActionNode*>{ { _id } };
}

ArrayNode::ArrayNode(const Token& token, const std::vector<ActionNode*>& values):
	ActionNode(token),
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
	ActionNode(decl->GetToken()),
	_declaration(decl)
{
}

std::queue<ActionNode*> VariableNode::WalkDown() const
{
	return _declaration->WalkDown();
}

VariableDeclarationNode::VariableDeclarationNode(const Token& token, ActionNode* value):
	ActionNode(token),
	_value(value)
{
}

std::queue<ActionNode*> VariableDeclarationNode::WalkDown() const
{
	return std::queue<ActionNode*>{ { _value } };
}

UnaryNode::UnaryNode(const Token& token, ActionNode* child):
	ActionNode(token),
	_child(child)
{
}

std::queue<ActionNode*> UnaryNode::WalkDown() const
{
	return std::queue<ActionNode*>({ _child });
}

BinaryNode::BinaryNode(const Token& token, ActionNode* left, ActionNode* right):
	ActionNode(token),
	_left(left),
	_right(right)
{
}

std::queue<ActionNode*> BinaryNode::WalkDown() const
{
	return std::queue<ActionNode*>({ _left, _right });
}

FunctionCallNode::FunctionCallNode(FunctionDeclarationNode* root, std::vector<ActionNode*> arguments):
	ActionNode(root->GetToken()),
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

FunctionSignature::FunctionSignature(const Token& token, const std::vector<VariableDeclarationNode*>& args):
	_token(token),
	_arguments(args)
{
}

FunctionDeclarationNode::FunctionDeclarationNode(const FunctionSignature& signature, ActionNode* body):
	ActionNode(signature.GetToken()),
	_signature(signature),
	_body(body)
{
}

std::queue<ActionNode*> FunctionDeclarationNode::WalkDown() const
{
	return std::queue<ActionNode*>{{_body}};
}

ActionNodeFactory operator+(const ActionNodeFactory& lhs, const ActionNodeFactory& rhs)
{
	ActionNodeFactory newFactory;
	newFactory._functions = lhs._functions;
	newFactory._nodes = lhs._nodes;
	newFactory._variables = lhs._variables;
	newFactory._declarationOrder = lhs._declarationOrder;
	
	newFactory += rhs;
	
	return newFactory;
}

ActionNodeFactory& operator+=(ActionNodeFactory& lhs, const ActionNodeFactory& rhs)
{
	for (auto& f: rhs._functions)
		if (!lhs._functions.contains(f.first))
			lhs._functions[f.first] = f.second;
	
	for (auto& f: rhs._variables)
		if (!lhs._variables.contains(f.first))
			lhs._variables[f.first] = f.second;
	
	for (auto& f: rhs._nodes)
		lhs._nodes.push_back(f);
	
	for (auto& f: rhs._declarationOrder)
		lhs._declarationOrder.push_back(f);
	
	return lhs;
}
}
