#pragma once
#include <queue>

#include "Token.h"


class ActionNode
{
public:
	virtual ~ActionNode() = default;
	
	virtual std::queue<ActionNode*> WalkDown() const;
};


class DoubleNumberNode: public ActionNode
{
public:
	DoubleNumberNode(double number);
	
	double Value() const { return _value; }
	
	
private:
	double _value = 0.0;
};


class IntNumberNode: public ActionNode
{
public:
	IntNumberNode(int number);
	
	int Value() const { return _value; }
	
	
private:
	int _value = 0;
};


class ArrayGetterNode: public ActionNode
{
public:
	ArrayGetterNode(const std::string& name, ActionNode* id);

private:
	std::string _name;
	ActionNode* _id;
};

class ArrayDeclarationNode: public ActionNode
{
public:
	ArrayDeclarationNode(const std::string& name, std::vector<ActionNode*> values);
	
	const std::vector<ActionNode*>& Values() const { return _values; }
	
	
private:
	std::string _name;
	std::vector<ActionNode*> _values;
};


class VariableNode: public ActionNode
{
public:
	VariableNode(const std::string& name);
	
	const std::string& Name() const { return _name; }
	
	
private:
	std::string _name;
};


class UnaryNode: public ActionNode
{
public:
	UnaryNode(const std::string& name, ActionNode* child);
	
	std::queue<ActionNode*> WalkDown() const override;
	
	ActionNode* Child() const { return _child; }
	const std::string& Name() const { return _name; }
	
	
private:
	std::string _name;
	ActionNode* _child;
};


class BinaryNode: public ActionNode
{
public:
	BinaryNode(const std::string& name, ActionNode* left, ActionNode* right);
	
	std::queue<ActionNode*> WalkDown() const override;
	
	ActionNode* Left() const { return _left; }
	ActionNode* Right() const { return _right; }
	const std::string& Name() const { return _name; }
	
	
private:
	std::string _name;
	ActionNode* _left, *_right;
};


class FunctionCallNode: public ActionNode
{
public:
	FunctionCallNode(const std::string& name, std::vector<ActionNode*> arguments);
	
	std::queue<ActionNode*> WalkDown() const override;
	
	const std::vector<ActionNode*>& Arguments() const { return _arguments; }
	const std::string& Name() const { return _name; }
	
	
private:
	std::vector<ActionNode*> _arguments;
	std::string _name;
};


class FunctionSignature
{
public:
	FunctionSignature(const std::string& name, const std::vector<std::string>& args);
	
	
private:
	std::string _name;
	std::vector<std::string> _arguments;
};

class FunctionDeclarationNode: public ActionNode
{
public:
	FunctionDeclarationNode(const FunctionSignature& signature, ActionNode* body);
	
	
private:
	FunctionSignature _signature;
	ActionNode* _body;
};